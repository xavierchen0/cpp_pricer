#ifndef _PRICER
#define _PRICER

#include "instruments/Option.h"
#include "market/Market.h"
#include <cstddef>
#include <stdexcept>

// Strategy Design Pattern
class IOptionPricer {
public:
  virtual ~IOptionPricer() = default;

  virtual double calculatePrice(const Market &market,
                                const Option &option) const = 0;
};

class CRRBinTreeOptionPricer final : public IOptionPricer {
public:
  CRRBinTreeOptionPricer(int timeSteps) : m_timeSteps{timeSteps} {
    if (m_timeSteps <= 0) {
      throw std::invalid_argument(
          "Error: Time steps must be strictly more than zero");
    }
  }

  double calculatePrice(const Market &market,
                        const Option &option) const override;

private:
  int m_timeSteps{};
};

#endif
