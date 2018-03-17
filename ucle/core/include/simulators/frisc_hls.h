#include <common/types.h>
#include <util/binary.h>
#include <simulators/hls.h>


namespace ucle {
    namespace hls {

        struct frisc_config : simulator_config {};

        class frisc_hls : high_level_simulator<simulator_state, frisc_config> {

        };

    }
}