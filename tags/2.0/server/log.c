#include "MyleftServer.h"

//日志写入
void log_write(int level, const char *msgfmt, char *file, int line, ...) {
    printf( "level:%i, msg:%s, file:%s, line:%i\n", level, msgfmt, file, line);
    if (fp && DEBUG) {
        int flag;
    
        char msg[255];
        snprintf(msg, sizeof (msg), "level:%i, msg:%s, file:%s, line:%i\n", level, msgfmt, file, line);
        flag = fwrite(msg, strlen(msg), 1, fp);
    }
}