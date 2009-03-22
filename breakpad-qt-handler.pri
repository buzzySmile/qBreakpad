unix:!mac {
	debug {
		# google-breakpad supports only stabs symbols on GNU/Linux and *BSD for now
		QMAKE_CXXFLAGS_DEBUG +=-gstabs
	}
}

# test config
LIST = thread exceptions rtti stl
for(f, LIST) {
	!CONFIG($$f) {
		warning("Add '$$f' to CONFIG, or you will find yourself in 'funny' problems.")
	}
}
