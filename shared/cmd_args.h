namespace cmd_args {
	int   load_int (int argc, char **argv, char* name, int preset);
	bool  load_bool(int argc, char **argv, char* name);
	char* load_str (int argc, char **argv, char* name, char* preset);
}
