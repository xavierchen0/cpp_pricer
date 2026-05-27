#include "Market.h"


void RateCurve::display() const {
    std::cout << "rate curve:" << name << std::endl;
    for (size_t i=0; i<tenorDates.size(); i++) {
      std::cout << tenorDates[i] << ":" << rates[i] << std::endl;
  }
  std::cout << std::endl;
}

void RateCurve::addRate(Date tenor, double rate) {
  //consider to check if tenor already exist
  if (true){
      tenorDates.push_back(tenor);
      rates.push_back(rate);
  }  
}

double RateCurve::getRate(Date tenor) const {
  //use linear interpolation to get rate
  return 0;

}

void VolCurve::display() const {

}

void Market::Print() const
{
  std::cout << "market asof: " << asOf << std::endl;
  
  for (auto curve: curves) {
    curve.second.display();
  }
  for (auto vol: vols) {
    vol.second.display();
  }
  /*
  add display for bond price and stock price
  
  */
}

void Market::addCurve(const std::string& curveName, const RateCurve& curve){
  curves.emplace(curveName, curve);
}

std::ostream& operator<<(std::ostream& os, const Market& mkt)
{
  os << mkt.asOf << std::endl;
  return os;
}

std::istream& operator>>(std::istream& is, Market& mkt)
{
  is >> mkt.asOf;
  return is;
}
