
#pragma once

#include <ostream>

namespace uoslibs {

    namespace uostest {
        void hello();

        class for_hello {
        public:
            int k;

            friend std::ostream &operator<<(std::ostream &, uoslibs::uostest::for_hello);
        };
    }
}
