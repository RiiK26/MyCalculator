#include "ProgrammerEngine.h"

ProgrammerEngine::ProgrammerEngine(QObject* parent) :
    QObject(parent),
    currentValue(0),
    pendingOp(""),
    waitingForNewOperand(true),
    displayValue("0"),
    historyValue(""),
    currentBase(10)
{
}

void ProgrammerEngine::processInput(const QString& text)
{
  if (QString("/ * - +").contains(text)) {
    if (!waitingForNewOperand) {
      calculate();
    }
    pendingOp            = text;
    currentValue         = displayValue.toLongLong(nullptr, currentBase);
    waitingForNewOperand = true;
    historyValue         = QString::number(currentValue, currentBase).toUpper() + " " + pendingOp;
    displayValue         = "0";
  }
  else {
    if (waitingForNewOperand) {
      displayValue.clear();
      waitingForNewOperand = false;
    }
    displayValue += text;
  }
  emit displayUpdated();
}

void ProgrammerEngine::calculate()
{
  if (pendingOp.isEmpty())
    return;
  qint64 operand = displayValue.toLongLong(nullptr, currentBase);
  qint64 result  = 0;

  if (pendingOp == "+")
    result = currentValue + operand;
  else if (pendingOp == "-")
    result = currentValue - operand;
  else if (pendingOp == "*")
    result = currentValue * operand;
  else if (pendingOp == "/") {
    if (operand == 0) {
      displayValue         = "Error";
      pendingOp            = "";
      waitingForNewOperand = true;
      emit displayUpdated();
      return;
    }
    result = currentValue / operand;
  }

  displayValue         = QString::number(result, currentBase).toUpper();
  historyValue         = "";
  pendingOp            = "";
  waitingForNewOperand = true;
  currentValue         = result;
  emit displayUpdated();
}

void ProgrammerEngine::clear()
{
  currentValue         = 0;
  pendingOp            = "";
  waitingForNewOperand = true;
  displayValue         = "0";
  historyValue         = "";
  emit displayUpdated();
}

void ProgrammerEngine::backspace()
{
  if (waitingForNewOperand)
    return;
  displayValue.chop(1);
  if (displayValue.isEmpty() || displayValue == "-") {
    displayValue         = "0";
    waitingForNewOperand = true;
  }
  emit displayUpdated();
}

void ProgrammerEngine::changeBase(int index)
{
  qint64 val = displayValue.toLongLong(nullptr, currentBase);

  int newBase = 10;
  if (index == 0)
    newBase = 16;
  else if (index == 1)
    newBase = 10;
  else if (index == 2)
    newBase = 8;
  else if (index == 3)
    newBase = 2;

  displayValue = QString::number(val, newBase).toUpper();

  if (!historyValue.isEmpty()) {
    historyValue = QString::number(currentValue, newBase).toUpper() + " " + pendingOp;
  }

  currentBase = newBase;
  emit displayUpdated();
}

QString ProgrammerEngine::getDisplayValue() const { return displayValue; }

QString ProgrammerEngine::getHistoryValue() const { return historyValue; }

int ProgrammerEngine::getCurrentBase() const { return currentBase; }
