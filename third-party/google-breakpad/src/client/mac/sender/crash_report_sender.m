// Copyright (c) 2006, Google Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#import <pwd.h>
#import <sys/stat.h>
#import <unistd.h>

#import <Cocoa/Cocoa.h>
#import <SystemConfiguration/SystemConfiguration.h>

#import "common/mac/HTTPMultipartUpload.h"

#import "crash_report_sender.h"
#import "common/mac/GTMLogger.h"

#define kLastSubmission @"LastSubmission"
const int kMinidumpFileLengthLimit = 800000;

#define kApplePrefsSyncExcludeAllKey @"com.apple.PreferenceSync.ExcludeAllSyncKeys"

@interface Reporter(PrivateMethods)
+ (uid_t)consoleUID;

- (id)initWithConfigurationFD:(int)fd;

- (NSString *)readString;
- (NSData *)readData:(ssize_t)length;

- (BOOL)readConfigurationData;
- (BOOL)readMinidumpData;
- (BOOL)readLogFileData;

- (BOOL)askUserPermissionToSend:(BOOL)shouldSubmitReport;
- (BOOL)shouldSubmitReport;

// Run an alert window with the given timeout. Returns NSAlertButtonDefault if
// the timeout is exceeded. A timeout of 0 queues the message immediately in the
// modal run loop.
- (int)runModalWindow:(NSWindow*)window withTimeout:(NSTimeInterval)timeout;

- (NSString*)clientID;
@end

@implementation Reporter
//=============================================================================
+ (uid_t)consoleUID {
  SCDynamicStoreRef store =
    SCDynamicStoreCreate(kCFAllocatorDefault, CFSTR("Reporter"), NULL, NULL);
  uid_t uid = -2;  // Default to "nobody"
  if (store) {
    CFStringRef user = SCDynamicStoreCopyConsoleUser(store, &uid, NULL);

    if (user)
      CFRelease(user);
    else
      uid = -2;

    CFRelease(store);
  }

  return uid;
}

//=============================================================================
- (id)initWithConfigurationFD:(int)fd {
  if ((self = [super init])) {
    configFile_ = fd;
  }

  // Because the reporter is embedded in the framework (and many copies
  // of the framework may exist) its not completely certain that the OS
  // will obey the com.apple.PreferenceSync.ExcludeAllSyncKeys in our
  // Info.plist. To make sure, also set the key directly if needed.
  NSUserDefaults *ud = [NSUserDefaults standardUserDefaults];
  if (![ud boolForKey:kApplePrefsSyncExcludeAllKey]) {
    [ud setBool:YES forKey:kApplePrefsSyncExcludeAllKey];
  }

  return self;
}

//=============================================================================
- (NSString *)readString {
  NSMutableString *str = [NSMutableString stringWithCapacity:32];
  char ch[2] = { 0 };

  while (read(configFile_, &ch[0], 1) == 1) {
    if (ch[0] == '\n') {
      // Break if this is the first newline after reading some other string
      // data.
      if ([str length])
        break;
    } else {
      [str appendString:[NSString stringWithUTF8String:ch]];
    }
  }

  return str;
}

//=============================================================================
- (NSData *)readData:(ssize_t)length {
  NSMutableData *data = [NSMutableData dataWithLength:length];
  char *bytes = (char *)[data bytes];

  if (read(configFile_, bytes, length) != length)
    return nil;

  return data;
}

//=============================================================================
- (BOOL)readConfigurationData {
  parameters_ = [[NSMutableDictionary alloc] init];

  while (1) {
    NSString *key = [self readString];

    if (![key length])
      break;

    // Read the data.  Try to convert to a UTF-8 string, or just save
    // the data
    NSString *lenStr = [self readString];
    ssize_t len = [lenStr intValue];
    NSData *data = [self readData:len];
    id value = [[NSString alloc] initWithData:data
                                     encoding:NSUTF8StringEncoding];

    [parameters_ setObject:value ? value : data forKey:key];
    [value release];
  }

  // generate a unique client ID based on this host's MAC address
  // then add a key/value pair for it
  NSString *clientID = [self clientID];
  [parameters_ setObject:clientID forKey:@"guid"];

  close(configFile_);
  configFile_ = -1;

  return YES;
}

// Per user per machine
- (NSString *)clientID {
  NSUserDefaults *ud = [NSUserDefaults standardUserDefaults];
  NSString *crashClientID = [ud stringForKey:kClientIdPreferenceKey];
  if (crashClientID) {
    return crashClientID;
  }

  // Otherwise, if we have no client id, generate one!
  srandom([[NSDate date] timeIntervalSince1970]);
  long clientId1 = random();
  long clientId2 = random();
  long clientId3 = random();
  crashClientID = [NSString stringWithFormat:@"%x%x%x",
                            clientId1, clientId2, clientId3];

  [ud setObject:crashClientID forKey:kClientIdPreferenceKey];
  [ud synchronize];
  return crashClientID;
}

//=============================================================================
- (BOOL)readLogFileData {
  unsigned int logFileCounter = 0;

  NSString *logPath;
  int logFileTailSize = [[parameters_ objectForKey:@BREAKPAD_LOGFILE_UPLOAD_SIZE]
                          intValue];

  NSMutableArray *logFilenames; // An array of NSString, one per log file
  logFilenames = [[NSMutableArray alloc] init];

  char tmpDirTemplate[80] = "/tmp/CrashUpload-XXXXX";
  char *tmpDir = mkdtemp(tmpDirTemplate);

  // Construct key names for the keys we expect to contain log file paths
  for(logFileCounter = 0;; logFileCounter++) {
    NSString *logFileKey = [NSString stringWithFormat:@"%@%d",
                                     @BREAKPAD_LOGFILE_KEY_PREFIX,
                                     logFileCounter];

    logPath = [parameters_ objectForKey:logFileKey];

    // They should all be consecutive, so if we don't find one, assume
    // we're done

    if (!logPath) {
      break;
    }

    NSData *entireLogFile = [[NSData alloc] initWithContentsOfFile:logPath];

    if (entireLogFile == nil) {
      continue;
    }

    NSRange fileRange;

    // Truncate the log file, only if necessary

    if ([entireLogFile length] <= logFileTailSize) {
      fileRange = NSMakeRange(0, [entireLogFile length]);
    } else {
      fileRange = NSMakeRange([entireLogFile length] - logFileTailSize,
                              logFileTailSize);
    }

    char tmpFilenameTemplate[100];

    // Generate a template based on the log filename
    sprintf(tmpFilenameTemplate,"%s/%s-XXXX", tmpDir,
            [[logPath lastPathComponent] fileSystemRepresentation]);

    char *tmpFile = mktemp(tmpFilenameTemplate);

    NSData *logSubdata = [entireLogFile subdataWithRange:fileRange];
    NSString *tmpFileString = [NSString stringWithUTF8String:tmpFile];
    [logSubdata writeToFile:tmpFileString atomically:NO];

    [logFilenames addObject:[tmpFileString lastPathComponent]];
    [entireLogFile release];
  }

  if ([logFilenames count] == 0) {
    [logFilenames release];
    logFileData_ =  nil;
    return NO;
  }

  // now, bzip all files into one
  NSTask *tarTask = [[NSTask alloc] init];

  [tarTask setCurrentDirectoryPath:[NSString stringWithUTF8String:tmpDir]];
  [tarTask setLaunchPath:@"/usr/bin/tar"];

  NSMutableArray *bzipArgs = [NSMutableArray arrayWithObjects:@"-cjvf",
                                             @"log.tar.bz2",nil];
  [bzipArgs addObjectsFromArray:logFilenames];

  [logFilenames release];

  [tarTask setArguments:bzipArgs];
  [tarTask launch];
  [tarTask waitUntilExit];
  [tarTask release];

  NSString *logTarFile = [NSString stringWithFormat:@"%s/log.tar.bz2",tmpDir];
  logFileData_ = [[NSData alloc] initWithContentsOfFile:logTarFile];
  if (logFileData_ == nil) {
    GTMLoggerDebug(@"Cannot find temp tar log file: %@", logTarFile);
    return NO;
  }
  return YES;

}

//=============================================================================
- (BOOL)readMinidumpData {
  NSString *minidumpDir = [parameters_ objectForKey:@kReporterMinidumpDirectoryKey];
  NSString *minidumpID = [parameters_ objectForKey:@kReporterMinidumpIDKey];

  if (![minidumpID length])
    return NO;

  NSString *path = [minidumpDir stringByAppendingPathComponent:minidumpID];
  path = [path stringByAppendingPathExtension:@"dmp"];

  // check the size of the minidump and limit it to a reasonable size
  // before attempting to load into memory and upload
  const char *fileName = [path fileSystemRepresentation];
  struct stat fileStatus;

  BOOL success = YES;

  if (!stat(fileName, &fileStatus)) {
    if (fileStatus.st_size > kMinidumpFileLengthLimit) {
      fprintf(stderr, "Breakpad Reporter: minidump file too large " \
              "to upload : %d\n", (int)fileStatus.st_size);
      success = NO;
    }
  } else {
      fprintf(stderr, "Breakpad Reporter: unable to determine minidump " \
              "file length\n");
      success = NO;
  }

  if (success) {
    minidumpContents_ = [[NSData alloc] initWithContentsOfFile:path];
    success = ([minidumpContents_ length] ? YES : NO);
  }

  if (!success) {
    // something wrong with the minidump file -- delete it
    unlink(fileName);
  }

  return success;
}

//=============================================================================
- (BOOL)askUserPermissionToSend:(BOOL)shouldSubmitReport {
  // Send without confirmation
  if ([[parameters_ objectForKey:@BREAKPAD_SKIP_CONFIRM] isEqualToString:@"YES"]) {
    GTMLoggerDebug(@"Skipping confirmation and sending report");
    return YES;
  }

  // Determine if we should create a text box for user feedback
  BOOL shouldRequestComments =
      [[parameters_ objectForKey:@BREAKPAD_REQUEST_COMMENTS]
        isEqual:@"YES"];

  NSString *display = [parameters_ objectForKey:@BREAKPAD_PRODUCT_DISPLAY];

  if (![display length])
    display = [parameters_ objectForKey:@BREAKPAD_PRODUCT];

  NSString *vendor = [parameters_ objectForKey:@BREAKPAD_VENDOR];

  if (![vendor length])
    vendor = @"Vendor not specified";

  NSBundle *bundle = [NSBundle mainBundle];
  [self setHeaderMessage:[NSString stringWithFormat:
                          NSLocalizedStringFromTableInBundle(@"headerFmt", nil,
                                                             bundle,
                                                             @""), display]];
  NSString *defaultButtonTitle = nil;
  NSString *otherButtonTitle = nil;
  NSTimeInterval timeout = 60.0;  // timeout value for the user notification

  // Get the localized alert strings
  // If we're going to submit a report, prompt the user if this is okay.
  // Otherwise, just let them know that the app crashed.

  if (shouldSubmitReport) {
    NSString *msgFormat = NSLocalizedStringFromTableInBundle(@"msg",
                                                             nil,
                                                             bundle, @"");

    [self setReportMessage:[NSString stringWithFormat:msgFormat, vendor]];

    defaultButtonTitle = NSLocalizedStringFromTableInBundle(@"sendReportButton",
                                                            nil, bundle, @"");
    otherButtonTitle = NSLocalizedStringFromTableInBundle(@"cancelButton", nil,
                                                          bundle, @"");

    // Nominally use the report interval
    timeout = [[parameters_ objectForKey:@BREAKPAD_REPORT_INTERVAL]
                floatValue];
  } else {
    [self setReportMessage:NSLocalizedStringFromTableInBundle(@"noSendMsg", nil,
                                                              bundle, @"")];
    defaultButtonTitle = NSLocalizedStringFromTableInBundle(@"noSendButton",
                                                            nil, bundle, @"");
    timeout = 60.0;
  }
  // show the notification for at least one minute
  if (timeout < 60.0) {
    timeout = 60.0;
  }

  // Initialize Cocoa, needed to display the alert
  NSApplicationLoad();

  int buttonPressed = NSAlertAlternateReturn;

  if (shouldRequestComments) {
    BOOL didLoadNib = [NSBundle loadNibNamed:@"Breakpad" owner:self];
    if (didLoadNib) {
      // Append the request for comments to the |reportMessage| string.
      NSString *commentsMessage =
          NSLocalizedStringFromTableInBundle(@"commentsMsg", nil, bundle, @"");
      [self setReportMessage:[NSString stringWithFormat:@"%@\n\n%@",
                              [self reportMessage],
                              commentsMessage]];

      // Add the request for email address.
      [self setEmailMessage:
          NSLocalizedStringFromTableInBundle(@"emailMsg", nil, bundle, @"")];

      // Run the alert
      buttonPressed = [self runModalWindow:alertWindow withTimeout:timeout];

      // Extract info from the user into the parameters_ dictionary
      if ([self commentsValue]) {
        [parameters_ setObject:[self commentsValue]
                      forKey:@BREAKPAD_COMMENTS];
      }
      if ([self emailValue]) {
        [parameters_ setObject:[self emailValue]
                        forKey:@BREAKPAD_EMAIL];
      }
    }
  } else {
    // Create an alert panel to tell the user something happened
    NSPanel* alert = NSGetAlertPanel([self headerMessage],
                                     [self reportMessage],
                                     defaultButtonTitle,
                                     otherButtonTitle, nil);

    // Pop the alert with an automatic timeout, and wait for the response
    buttonPressed = [self runModalWindow:alert withTimeout:timeout];

    // Release the panel memory
    NSReleaseAlertPanel(alert);
  }
  return buttonPressed == NSAlertDefaultReturn;
}

- (int)runModalWindow:(NSWindow*)window withTimeout:(NSTimeInterval)timeout {
  // Queue a |stopModal| message to be performed in |timeout| seconds.
  [NSApp performSelector:@selector(stopModal)
              withObject:nil
              afterDelay:timeout];

  // Run the window modally and wait for either a |stopModal| message or a
  // button click.
  [NSApp activateIgnoringOtherApps:YES];
  int returnMethod = [NSApp runModalForWindow:window];

  // Cancel the pending |stopModal| message.
  if (returnMethod != NSRunStoppedResponse) {
    [NSObject cancelPreviousPerformRequestsWithTarget:NSApp
                                             selector:@selector(stopModal)
                                               object:nil];
  }
  return returnMethod;
}

- (IBAction)sendReport:(id)sender {
  [alertWindow orderOut:self];
  // Use NSAlertDefaultReturn so that the return value of |runModalWithWindow|
  // matches the AppKit function NSRunAlertPanel()
  [NSApp stopModalWithCode:NSAlertDefaultReturn];
}

// UI Button Actions
//=============================================================================
- (IBAction)cancel:(id)sender {
  [alertWindow orderOut:self];
  // Use NSAlertDefaultReturn so that the return value of |runModalWithWindow|
  // matches the AppKit function NSRunAlertPanel()
  [NSApp stopModalWithCode:NSAlertAlternateReturn];
}

- (IBAction)showPrivacyPolicy:(id)sender {
  // Get the localized privacy policy URL and open it in the default browser.
  NSURL* privacyPolicyURL =
      [NSURL URLWithString:NSLocalizedStringFromTableInBundle(
          @"privacyPolicyURL", nil, [NSBundle mainBundle], @"")];
  [[NSWorkspace sharedWorkspace] openURL:privacyPolicyURL];
}

// Text Field Delegate Methods
//=============================================================================
- (BOOL)control:(NSControl*)control textView:(NSTextView*)textView
                         doCommandBySelector:(SEL)commandSelector {
  BOOL result = NO;
  // If the user has entered text, don't end editing on "return"
  if (commandSelector == @selector(insertNewline:)
      && [[textView string] length] > 0) {
    [textView insertNewlineIgnoringFieldEditor:self];
    result = YES;
  }
  return result;
}

// Accessors
//=============================================================================
- (NSString *)headerMessage {
  return [[headerMessage_ retain] autorelease];
}

- (void)setHeaderMessage:(NSString *)value {
  if (headerMessage_ != value) {
    [headerMessage_ autorelease];
    headerMessage_ = [value copy];
  }
}

- (NSString *)reportMessage {
  return [[reportMessage_ retain] autorelease];
}

- (void)setReportMessage:(NSString *)value {
  if (reportMessage_ != value) {
    [reportMessage_ autorelease];
    reportMessage_ = [value copy];
  }
}

- (NSString *)commentsValue {
  return [[commentsValue_ retain] autorelease];
}

- (void)setCommentsValue:(NSString *)value {
  if (commentsValue_ != value) {
    [commentsValue_ autorelease];
    commentsValue_ = [value copy];
  }
}

- (NSString *)emailMessage {
  return [[emailMessage_ retain] autorelease];
}

- (void)setEmailMessage:(NSString *)value {
  if (emailMessage_ != value) {
    [emailMessage_ autorelease];
    emailMessage_ = [value copy];
  }
}

- (NSString *)emailValue {
  return [[emailValue_ retain] autorelease];
}

- (void)setEmailValue:(NSString *)value {
  if (emailValue_ != value) {
    [emailValue_ autorelease];
    emailValue_ = [value copy];
  }
}

//=============================================================================
- (BOOL)shouldSubmitReport {
  float interval = [[parameters_ objectForKey:@BREAKPAD_REPORT_INTERVAL]
    floatValue];
  NSString *program = [parameters_ objectForKey:@BREAKPAD_PRODUCT];
  NSUserDefaults *ud = [NSUserDefaults standardUserDefaults];
  NSMutableDictionary *programDict =
    [NSMutableDictionary dictionaryWithDictionary:[ud dictionaryForKey:program]];
  NSNumber *lastTimeNum = [programDict objectForKey:kLastSubmission];
  NSTimeInterval lastTime = lastTimeNum ? [lastTimeNum floatValue] : 0;
  NSTimeInterval now = CFAbsoluteTimeGetCurrent();
  NSTimeInterval spanSeconds = (now - lastTime);

  [programDict setObject:[NSNumber numberWithFloat:now] forKey:kLastSubmission];
  [ud setObject:programDict forKey:program];
  [ud synchronize];

  // If we've specified an interval and we're within that time, don't ask the
  // user if we should report
  GTMLoggerDebug(@"Reporter Interval: %f", interval);
  if (interval > spanSeconds) {
    GTMLoggerDebug(@"Within throttling interval, not sending report");
    return NO;
  }
  return YES;
}

//=============================================================================
- (void)report {
  NSURL *url = [NSURL URLWithString:[parameters_ objectForKey:@BREAKPAD_URL]];
  HTTPMultipartUpload *upload = [[HTTPMultipartUpload alloc] initWithURL:url];
  NSMutableDictionary *uploadParameters = [NSMutableDictionary dictionary];

  // Set the known parameters.  This should be kept up to date with the
  // parameters defined in the Breakpad.h list of parameters.  The intent
  // is so that if there's a parameter that's not in this list, we consider
  // it to be a "user-defined" parameter and we'll upload it to the server.
  NSSet *knownParameters =
    [NSSet setWithObjects:@kReporterMinidumpDirectoryKey,
           @kReporterMinidumpIDKey, @BREAKPAD_PRODUCT_DISPLAY,
           @BREAKPAD_PRODUCT, @BREAKPAD_VERSION, @BREAKPAD_URL,
           @BREAKPAD_REPORT_INTERVAL, @BREAKPAD_SKIP_CONFIRM,
           @BREAKPAD_SEND_AND_EXIT, @BREAKPAD_REPORTER_EXE_LOCATION,
           @BREAKPAD_INSPECTOR_LOCATION, @BREAKPAD_LOGFILES,
           @BREAKPAD_LOGFILE_UPLOAD_SIZE, @BREAKPAD_EMAIL,
           @BREAKPAD_REQUEST_COMMENTS, @BREAKPAD_COMMENTS,
           @BREAKPAD_VENDOR, nil];

  // Add parameters
  [uploadParameters setObject:[parameters_ objectForKey:@BREAKPAD_PRODUCT]
                    forKey:@"prod"];
  [uploadParameters setObject:[parameters_ objectForKey:@BREAKPAD_VERSION]
                    forKey:@"ver"];

  if ([parameters_ objectForKey:@BREAKPAD_EMAIL]) {
    [uploadParameters setObject:[parameters_ objectForKey:@BREAKPAD_EMAIL] forKey:@"email"];
  }

  NSString* comments = [parameters_ objectForKey:@BREAKPAD_COMMENTS];
  if (comments != nil) {
    [uploadParameters setObject:comments forKey:@"comments"];
  }
  // Add any user parameters
  NSArray *parameterKeys = [parameters_ allKeys];
  int keyCount = [parameterKeys count];
  for (int i = 0; i < keyCount; ++i) {
    NSString *key = [parameterKeys objectAtIndex:i];
    if (![knownParameters containsObject:key] &&
        ![key hasPrefix:@BREAKPAD_LOGFILE_KEY_PREFIX])
      [uploadParameters setObject:[parameters_ objectForKey:key] forKey:key];
  }
  [upload setParameters:uploadParameters];
  // Add minidump file
  if (minidumpContents_) {
    [upload addFileContents:minidumpContents_ name:@"upload_file_minidump"];

    // Send it
    NSError *error = nil;
    NSData *data = [upload send:&error];
    NSString *result = [[NSString alloc] initWithData:data
                                         encoding:NSUTF8StringEncoding];
    const char *reportID = "ERR";

    if (error) {
      fprintf(stderr, "Breakpad Reporter: Send Error: %s\n",
              [[error description] UTF8String]);
    } else {
      NSCharacterSet *trimSet = [NSCharacterSet whitespaceAndNewlineCharacterSet];
      reportID = [[result stringByTrimmingCharactersInSet:trimSet] UTF8String];
    }

    // rename the minidump file according to the id returned from the server
    NSString *minidumpDir = [parameters_ objectForKey:@kReporterMinidumpDirectoryKey];
    NSString *minidumpID = [parameters_ objectForKey:@kReporterMinidumpIDKey];

    NSString *srcString = [NSString stringWithFormat:@"%@/%@.dmp",
                                    minidumpDir, minidumpID];
    NSString *destString = [NSString stringWithFormat:@"%@/%s.dmp",
                                     minidumpDir, reportID];

    const char *src = [srcString fileSystemRepresentation];
    const char *dest = [destString fileSystemRepresentation];

    if (rename(src, dest) == 0) {
      fprintf(stderr, "Breakpad Reporter: Renamed %s to %s after successful " \
              "upload\n",src, dest);
    }
    else {
      // can't rename - don't worry - it's not important for users
      fprintf(stderr, "Breakpad Reporter: successful upload report ID = %s\n",
              reportID );
    }
    [result release];
  }

  if (logFileData_) {
    HTTPMultipartUpload *logUpload = [[HTTPMultipartUpload alloc] initWithURL:url];

    [uploadParameters setObject:@"log" forKey:@"type"];
    [logUpload setParameters:uploadParameters];
    [logUpload addFileContents:logFileData_ name:@"log"];

    NSError *error = nil;
    NSData *data = [logUpload send:&error];
    NSString *result = [[NSString alloc] initWithData:data
                                         encoding:NSUTF8StringEncoding];
    [result release];
    [logUpload release];
  }

  [upload release];
}

//=============================================================================
- (void)dealloc {
  [parameters_ release];
  [minidumpContents_ release];
  [logFileData_ release];
  [super dealloc];
}
@end

//=============================================================================
int main(int argc, const char *argv[]) {
  NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
  // The expectation is that there will be one argument which is the path
  // to the configuration file
  if (argc != 2) {
    exit(1);
  }

  // Open the file before (potentially) switching to console user
  int configFile = open(argv[1], O_RDONLY, 0600);

  // we want to avoid a build-up of old config files even if they
  // have been incorrectly written by the framework
  unlink(argv[1]);

  if (configFile == -1) {
    exit(1);
  }

  Reporter *reporter = [[Reporter alloc] initWithConfigurationFD:configFile];

  // Gather the configuration data
  if (![reporter readConfigurationData]) {
    exit(1);
  }

  // Read the minidump into memory before we (potentially) switch from the
  // root user
  [reporter readMinidumpData];

  [reporter readLogFileData];

  // only submit a report if we have not recently crashed in the past
  BOOL shouldSubmitReport = [reporter shouldSubmitReport];
  BOOL okayToSend = NO;

  // ask user if we should send
  if (shouldSubmitReport) {
    okayToSend = [reporter askUserPermissionToSend:shouldSubmitReport];
  }

  // If we're running as root, switch over to nobody
  if (getuid() == 0 || geteuid() == 0) {
    struct passwd *pw = getpwnam("nobody");

    // If we can't get a non-root uid, don't send the report
    if (!pw)
      exit(0);

    if (setgid(pw->pw_gid) == -1)
      exit(0);

    if (setuid(pw->pw_uid) == -1)
      exit(0);
  }

  if (okayToSend && shouldSubmitReport) {
    [reporter report];
  }

  // Cleanup
  [reporter release];
  [pool release];

  return 0;
}
