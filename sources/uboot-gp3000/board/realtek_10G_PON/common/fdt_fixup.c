#include <common.h>
#include <linux/libfdt.h>
#include <fdt_support.h>
#include <linux/string.h>
#include <malloc.h>

static char *fdt_collect_props(void *fdt, int offset) {
	int level;
	int nodeoffset;
	int nextoffset;
	uint32_t tag;
	const char *prop;
	const struct fdt_property *fdt_prop;
	const int max_level = 1;
	const int props_size = 1024;
	size_t n;

	char *all_props, *p;
	p = all_props = malloc(props_size * sizeof(char));
	if (all_props == NULL) {
		return NULL;
	}

	memset(all_props, 0, props_size * sizeof(char));

	for (level = 0, nodeoffset = offset;;) {
		tag = fdt_next_tag(fdt, nodeoffset, &nextoffset);
		switch (tag) {
			case FDT_BEGIN_NODE:
				level += 1;
				break;
			case FDT_END_NODE:
				level -= 1;
				break;
			case FDT_PROP:
				if (level > max_level) {
					break;
				}
				fdt_prop = fdt_offset_ptr(fdt, nodeoffset, sizeof(*fdt_prop));
				prop = fdt_string(fdt, fdt32_to_cpu(fdt_prop->nameoff));
				n = strlen(prop);
				if (p + n + 1 >= all_props + props_size) {
					printf("FDT_Fixup: memory is not enough\n");
					return all_props;
				} else {
					strcpy(p, prop);
					p += n + 1;
				}
				break;
			case FDT_END:
				return all_props;
			default:
				break;
		}

		if (level <= 0) {
			return all_props;
		}
		nodeoffset = nextoffset;
	}

	return all_props;
}

int fdt_fixup(void *blob) {
	int nodeoffset;
	char *value;
	char *props, *one_prop;

	nodeoffset = fdt_path_offset(blob, "/ca-scfg/options");
	if (nodeoffset < 0) {
		return 0;
	}

	props = fdt_collect_props(blob, nodeoffset);
	if (props == NULL) {
		return 0;
	}

	for (one_prop = props; *one_prop; one_prop += strlen(one_prop) + 1) {
		value = env_get(one_prop);
		if (value == NULL) {
			continue;
		}

		nodeoffset = fdt_path_offset(blob, "/ca-scfg/options");
		fdt_setprop(blob, nodeoffset, one_prop, value, strlen(value));
	}

	free(props);
	return 0;
}

int ft_system_setup(void *blob, bd_t *bd) {
	return fdt_fixup(blob);
}
