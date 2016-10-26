//
// Copyright (c) 2013-2016 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cassert>
#include "ztest.hpp"

void
doDeflate1_zlib(
    int level, int windowBits, int strategy,
        std::string const& check)
{
    int result;
    std::string out;
    ::z_stream zs;
    std::memset(&zs, 0, sizeof(zs));

    result = deflateInit2(&zs,
        level,
        Z_DEFLATED,
        -windowBits,
        8,
        strategy);

    if(! result == Z_OK)
        goto err;
    out.resize(deflateBound(&zs,
        static_cast<uLong>(check.size())));
    zs.next_in = (Bytef*)check.data();
    zs.avail_in = static_cast<uInt>(check.size());
    zs.next_out = (Bytef*)out.data();
    zs.avail_out = static_cast<uInt>(out.size());
    {
        bool progress = true;
        for(;;)
        {
            result = deflate(&zs, Z_FULL_FLUSH);
            if( result == Z_BUF_ERROR ||
                result == Z_STREAM_END) // per zlib FAQ
                goto fin;
            if(! progress)
                goto err;
            progress = false;
        }
    }

fin:
    out.resize(zs.total_out);
    {
        z_inflator zi;
        auto const s = zi(out);
        assert(s == check);
    }

err:
    deflateEnd(&zs);
}

int
main(int ac, char const* av[])
{
    // level = 0, windowBits = 9, and strategy = 2
    doDeflate1_zlib(0, 9, 2, corpus1(512 * 1024));
    return 0;
}
