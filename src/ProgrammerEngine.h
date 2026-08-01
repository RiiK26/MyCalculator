#ifndef PROGRAMMERENGINE_H
#define PROGRAMMERENGINE_H

#include <QObject>
#include <QString>

class ProgrammerEngine: public QObject
{
  Q_OBJECT
public:
  explicit ProgrammerEngine(QObject* parent = nullptr);

  void processInput(const QString& text);
  void calculate();
  void clear();
  void backspace();
  void changeBase(int index);  // 0=Hex, 1=Dec, 2=Oct, 3=Bin

  QString getDisplayValue() const;
  QString getHistoryValue() const;
  int     getCurrentBase() const;

signals:
  void displayUpdated();

private:
  qint64  currentValue;
  QString pendingOp;
  bool    waitingForNewOperand;
  QString displayValue;
  QString historyValue;
  int     currentBase;  // e.g. 16, 10, 8, 2
};

#endif  // PROGRAMMERENGINE_H
