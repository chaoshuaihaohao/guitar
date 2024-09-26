#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	FILE *file;
	char filename[] = "song_juhuatai";

	struct simple_table table;
	memset(&table, 0, sizeof(struct simple_table));

	// 打开文件
	file = fopen(filename, "r");
	if (file == NULL) {
		perror("Error opening file");
		return EXIT_FAILURE;
	}
	// 读取文件直到末尾或数组满
	while (table.len < MAX_INPUT_SIZE && !feof(file)) {
		if (fscanf
		    (file, "%c %d %d\n", &table.input[table.len].symbol,
		     &table.input[table.len].duration,
		     &table.input[table.len].pitch) == 3) {
			// 成功读取一行
			table.len++;
		} else {
			// 读取失败或到达文件末尾
			break;
		}
	}

	// 关闭文件
	fclose(file);

	// 打印读取的数据
	mc_debug("Read %d entries from file.\n", table.len);
	for (int j = 0; j < table.len; j++) {
		mc_debug("%c %d %d\n", table.input[j].symbol,
			 table.input[j].duration, table.input[j].pitch);
	}

	return EXIT_SUCCESS;
}
