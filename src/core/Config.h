/* XMRig
 * Copyright 2010      Jeff Garzik <jgarzik@pobox.com>
 * Copyright 2012-2014 pooler      <pooler@litecoinpool.org>
 * Copyright 2014      Lucas Jones <https://github.com/lucasjones>
 * Copyright 2014-2016 Wolf9466    <https://github.com/OhGodAPet>
 * Copyright 2016      Jay D Dee   <jayddee246@gmail.com>
 * Copyright 2017-2018 XMR-Stak    <https://github.com/fireice-uk>, <https://github.com/psychocrypt>
 * Copyright 2016-2018 XMRig       <https://github.com/xmrig>, <support@xmrig.com>
 * Copyright 2018 MoneroOcean      <https://github.com/MoneroOcean>, <support@moneroocean.stream>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef XMRIG_CONFIG_H
#define XMRIG_CONFIG_H


#include <stdint.h>
#include <vector>


#include "common/config/CommonConfig.h"
#include "common/xmrig.h"
#include "rapidjson/fwd.h"
#include "workers/CpuThread.h"


class Addr;
class Url;


namespace xmrig {


class ConfigLoader;
class IThread;
class IWatcherListener;


/**
 * @brief The Config class
 *
 * Options with dynamic reload:
 *   colors
 *   debug
 *   verbose
 *   custom-diff (only for new connections)
 *   api/worker-id
 *   pools/
 */
class Config : public CommonConfig
{
public:
    enum ThreadsMode {
        Automatic,
        Simple,
        Advanced
    };


    Config();

    bool reload(const char *json);

    void getJSON(rapidjson::Document &doc) const override;

    inline AesMode aesMode() const                       { return m_aesMode; }
    inline AlgoVariant algoVariant() const               { return m_algoVariant; }
    inline Assembly assembly() const                     { return m_assembly; }
    inline bool isHugePages() const                      { return m_hugePages; }
    inline bool isShouldSave() const                     { return m_shouldSave && isAutoSave(); }
    inline int priority() const                          { return m_priority; }

    // access to m_threads taking into accoun that it is now separated for each perf algo
    inline const std::vector<IThread *> &threads(const xmrig::Algo algo = INVALID_ALGO) const {
        return m_threads[algo == INVALID_ALGO ? m_algorithm.algo() : algo].list;
    }
    inline int threadsCount(const xmrig::Algo algo = INVALID_ALGO) const {
        return m_threads[algo == INVALID_ALGO ? m_algorithm.algo() : algo].list.size();
    }
    inline int64_t affinity(const xmrig::Algo algo = INVALID_ALGO) const {
        return m_threads[algo == INVALID_ALGO ? m_algorithm.algo() : algo].mask;
    }
    inline ThreadsMode threadsMode(const xmrig::Algo algo = INVALID_ALGO) const {
        return m_threads[algo == INVALID_ALGO ? m_algorithm.algo() : algo].mode;
    }

    // access to perf algo results
    inline float get_algo_perf(const xmrig::PerfAlgo pa) const             { return m_algo_perf[pa]; }
    inline void set_algo_perf(const xmrig::PerfAlgo pa, const float value) { m_algo_perf[pa] = value; }

    static Config *load(int argc, char **argv, IWatcherListener *listener);

protected:
    bool finalize() override;
    bool parseBoolean(int key, bool enable) override;
    bool parseString(int key, const char *arg) override;
    bool parseUint64(int key, uint64_t arg) override;
    void parseJSON(const rapidjson::Document &doc) override;
    // parse specific perf algo (or generic) threads config
    void parseThreadsJSON(const rapidjson::Value &threads, xmrig::Algo);

private:
    bool parseInt(int key, int arg);

    AlgoVariant getAlgoVariant() const;
#   ifndef XMRIG_NO_AEON
    AlgoVariant getAlgoVariantLite() const;
#   endif


    struct Threads
    {
       inline Threads() : mask(-1L), count(0), mode(Automatic) {}

       int64_t mask;
       size_t count;
       std::vector<CpuThread::Data> cpu;
       std::vector<IThread *> list;
       ThreadsMode mode;
    };


    AesMode m_aesMode;
    AlgoVariant m_algoVariant;
    Assembly m_assembly;
    bool m_hugePages;
    bool m_safe;
    bool m_shouldSave;
    int m_maxCpuUsage;
    int m_priority;
    // threads config for each algo
    Threads m_threads[xmrig::Algo::ALGO_MAX];
    // perf algo hashrate results
    float m_algo_perf[xmrig::PerfAlgo::PA_MAX];
};

extern Config* pconfig;


} /* namespace xmrig */

#endif /* XMRIG_CONFIG_H */
