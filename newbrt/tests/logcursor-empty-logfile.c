/* -*- mode: C; c-basic-offset: 4 -*- */
#ident "Copyright (c) 2007, 2008 Tokutek Inc.  All rights reserved."

#include "test.h"

#include "includes.h"

const int N = 2;

#define dname __FILE__ ".dir"
#define rmrf "rm -rf " dname "/"

int
test_main (int argc, const char *argv[]) {
    default_parse_args(argc, argv);

    int r;
    system(rmrf);
    r = toku_os_mkdir(dname, S_IRWXU);    assert(r==0);
    TOKULOGGER logger;
    LSN lsn = ZERO_LSN;

    int helloseq = 0;

    // create N log files with a hello message
    for (int i=0; i<N; i++) {
        r = toku_logger_create(&logger);
        assert(r == 0);

        r = toku_logger_open(dname, logger);
        assert(r == 0);

        char str[32];
        sprintf(str, "hello%d", helloseq++);
        BYTESTRING bs0 = { .data = str, .len = strlen(str) };
        r = toku_log_timestamp(logger, &lsn, 0, 0, bs0);
        assert(r == 0);

        r = toku_logger_close(&logger);
        assert(r == 0);
    }    

    // create N empty log files
    for (int i=0; i<N; i++) {
        r = toku_logger_create(&logger);
        assert(r == 0);

        r = toku_logger_open(dname, logger);
        assert(r == 0);

        r = toku_logger_close(&logger);
        assert(r == 0);
    }

    // create N log files with a hello message
    for (int i=0; i<N; i++) {
        r = toku_logger_create(&logger);
        assert(r == 0);

        r = toku_logger_open(dname, logger);
        assert(r == 0);

        char str[32];
        sprintf(str, "hello%d", helloseq++);
        BYTESTRING bs0 = { .data = str, .len = strlen(str) };
        r = toku_log_timestamp(logger, &lsn, 0, 0, bs0);
        assert(r == 0);

        r = toku_logger_close(&logger);
        assert(r == 0);
    }

    // verify the log forwards
    TOKULOGCURSOR lc = NULL;
    struct log_entry *le;

    r = toku_logcursor_create(&lc, dname);
    assert(r == 0 && lc != NULL);

    helloseq = 0;
    for (int i=0; i<2*N; i++) {

        r = toku_logcursor_next(lc, &le);
        assert(r == 0 && le->cmd == LT_timestamp);
        char expect[32];
        sprintf(expect, "hello%d", helloseq++);
        assert(le->u.timestamp.comment.len == strlen(expect) && memcmp(le->u.timestamp.comment.data, expect, le->u.timestamp.comment.len) == 0);
    }

    r = toku_logcursor_next(lc, &le);
    assert(r != 0);

    r = toku_logcursor_destroy(&lc);
    assert(r == 0 && lc == NULL);

    // verify the log backwards
    r = toku_logcursor_create(&lc, dname);
    assert(r == 0 && lc != NULL);

    helloseq = 2*N;
    for (int i=0; i<2*N; i++) {

        r = toku_logcursor_prev(lc, &le);
        assert(r == 0 && le->cmd == LT_timestamp);
        char expect[32];
        sprintf(expect, "hello%d", --helloseq);
        assert(le->u.timestamp.comment.len == strlen(expect) && memcmp(le->u.timestamp.comment.data, expect, le->u.timestamp.comment.len) == 0);
    }

    r = toku_logcursor_prev(lc, &le);
    assert(r != 0);

    r = toku_logcursor_destroy(&lc);
    assert(r == 0 && lc == NULL);

    return 0;
}