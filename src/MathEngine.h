#ifndef MATHENGINE_H
#define MATHENGINE_H

#include <QString>

class MathEngine
{
public:
  MathEngine();

  void processInput(const QString& text);
  void calculate();
  void clear();
  void backspace();

  QString getDisplayValue() const;
  QString getHistoryValue() const;

private:
  double  currentValue;
  QString pendingOp;
  bool    waitingForNewOperand;
  QString displayValue;
  QString historyValue;
};

#endif  // MATHENGINE_H
