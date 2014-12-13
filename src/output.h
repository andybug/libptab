#ifndef OUTPUT_H
#define OUTPUT_H

struct format_desc {
	const char *horiz_div;
	const char *vert_div;
	const char *top_left_intersect;
	const char *top_middle_intersect;
	const char *top_right_intersect;
	const char *div_left_intersect;
	const char *div_middle_intersect;
	const char *div_right_intersect;
	const char *bottom_left_intersect;
	const char *bottom_middle_intersect;
	const char *bottom_right_intersect;
};

extern const struct format_desc ascii_format;

#endif
