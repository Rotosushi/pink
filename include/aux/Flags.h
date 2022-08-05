#pragma once
#include <bitset>

namespace pink {
  class Flags {
    private:
      enum Kind {
        LHSOfAssignment = 1,        
      };

      std::bitset<32> flags;

    public:
      Flags();
      ~Flags();

      bool OnTheLHSOfAssignment();
      bool OnTheLHSOfAssignment(bool state);
  };
}
