#include "MathEngine.h"

MathEngine::MathEngine() :
    currentValue(0.0),
    pendingOp(""),
    waitingForNewOperand(true),
    displayValue("0"),
    historyValue("")
{
}

void MathEngine::processInput(const QString& text)
{
  if (QString("/ * - +").contains(text)) {
    if (!waitingForNewOperand) {
      calculate();
    }
    pendingOp            = text;
    currentValue         = displayValue.toDouble();
    waitingForNewOperand = true;

    historyValue = QString::number(currentValue, 'g', 15) + " " + pendingOp;
    displayValue = "0";
  }
  else {
    if (waitingForNewOperand) {
      displayValue.clear();
      waitingForNewOperand = false;
    }
    if (text == "." && displayValue.contains(".")) {
      return;
    }
    displayValue += text;
  }
}

void MathEngine::calculate()
{
  if (pendingOp.isEmpty())
    return;
  double operand = displayValue.toDouble();
  double result  = 0.0;
  if (pendingOp == "+")
    result = currentValue + operand;
  else if (pendingOp == "-")
    result = currentValue - operand;
  else if (pendingOp == "*")
    result = currentValue * operand;
  else if (pendingOp == "/") {
    if (operand == 0.0) {
      displayValue         = "Error";
      pendingOp            = "";
      waitingForNewOperand = true;
      return;
    }
    result = currentValue / operand;
  }

  displayValue         = QString::number(result, 'g', 15);
  historyValue         = "";
  pendingOp            = "";
  waitingForNewOperand = true;
  currentValue         = result;
}

void MathEngine::clear()
{
  currentValue         = 0.0;
  pendingOp            = "";
  waitingForNewOperand = true;
  displayValue         = "0";
  historyValue         = "";
}

void MathEngine::backspace()
{
  if (waitingForNewOperand)
    return;
  displayValue.chop(1);
  if (displayValue.isEmpty() || displayValue == "-") {
    displayValue         = "0";
    waitingForNewOperand = true;
  }
}

QString MathEngine::getDisplayValue() const { return displayValue; }

QString MathEngine::getHistoryValue() const { return historyValue; }
