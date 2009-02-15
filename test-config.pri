LIST = thread exceptions rtti stl
for(f, LIST) {
	!CONFIG($$f) {
		warning("Add '$$f' to CONFIG, or you will find yourself in 'funny' problems.")
	}
}
