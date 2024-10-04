namespace dev {
	extern int fps;
	extern int triangle_ct;

	void update();

	extern bool is_freecam;
	void toggle_floating_camera();
	void teleport_to_floating_camera();
}
