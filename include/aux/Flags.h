#pragma once
#include <bitset>

namespace pink {
  class Flags {
    private:
      enum Kind {
        LHSOfAssignment,
        inAddressOf,
        inDereferencePtr,
      };

      std::bitset<32> flags;

    public:
      Flags();
      ~Flags();

      bool OnTheLHSOfAssignment();
      bool OnTheLHSOfAssignment(bool state);
      bool WithinAddressOf();
      bool WithinAddressOf(bool state);
      bool WithinDereferencePtr();
      bool WithinDereferencePtr(bool state);
  };
}
