//#pragma once
#ifndef TB_PROBE_H_
#define TB_PROBE_H_

namespace Tablebases {

    extern int TBLargest;

    void init (const std::string& path);
    int probe_wdl (Position& pos, int *success);
    int probe_dtz (Position& pos, int *success);
    bool root_probe (Position& pos, Value& TBScore);
    bool root_probe_wdl (Position& pos, Value& TBScore);

}

#endif
