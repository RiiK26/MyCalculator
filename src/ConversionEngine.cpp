#include "ConversionEngine.h"

#include <QStringList>

QString ConversionEngine::convertTemperature(double val, int fromIdx, int toIdx)
{
  double celsius = 0.0;

  // To Celsius
  if (fromIdx == 0)
    celsius = val;
  else if (fromIdx == 1)
    celsius = (val - 32.0) * 5.0 / 9.0;
  else if (fromIdx == 2)
    celsius = val - 273.15;

  double  result  = 0.0;
  QString unitStr = "";

  // From Celsius
  if (toIdx == 0) {
    result  = celsius;
    unitStr = "Celsius";
  }
  else if (toIdx == 1) {
    result  = (celsius * 9.0 / 5.0) + 32.0;
    unitStr = "Fahrenheit";
  }
  else if (toIdx == 2) {
    result  = celsius + 273.15;
    unitStr = "Kelvin";
  }

  return QString("Result: %1 %2").arg(result, 0, 'f', 2).arg(unitStr);
}

QString ConversionEngine::convertNumber(const QString& text, int fromIdx, int toIdx)
{
  if (text.isEmpty()) {
    return "Result: ";
  }

  auto getBase = [](int idx) -> int {
    if (idx == 0)
      return 10;
    if (idx == 1)
      return 2;
    if (idx == 2)
      return 8;
    if (idx == 3)
      return 16;
    return 0;  // Text
  };

  int fromBase = getBase(fromIdx);
  int toBase   = getBase(toIdx);

  if (fromBase == 0 && toBase == 0) {
    return "Result: " + text;
  }
  else if (fromBase == 0) {
    // Text to Number
    QString res;
    for (QChar c : text) {
      res += QString::number(c.unicode(), toBase) + " ";
    }
    return "Result: " + res.trimmed().toUpper();
  }
  else if (toBase == 0) {
    // Number to Text
    QStringList parts = text.split(" ", Qt::SkipEmptyParts);
    QString     res;
    for (const QString& p : parts) {
      bool ok;
      int  unicode = p.toInt(&ok, fromBase);
      if (ok)
        res += QChar(unicode);
    }
    return "Result: " + res;
  }
  else {
    // Number to Number
    bool   ok;
    qint64 val = text.toLongLong(&ok, fromBase);
    if (ok) {
      return "Result: " + QString::number(val, toBase).toUpper();
    }
    else {
      return "Result: Error";
    }
  }
}
