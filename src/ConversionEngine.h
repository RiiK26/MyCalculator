#ifndef CONVERSIONENGINE_H
#define CONVERSIONENGINE_H

#include <QString>

class ConversionEngine
{
public:
  static QString convertTemperature(double value, int fromUnit, int toUnit);
  static QString convertNumber(const QString& value, int fromBaseIdx, int toBaseIdx);
};

#endif  // CONVERSIONENGINE_H
