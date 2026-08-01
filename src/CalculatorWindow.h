#ifndef CALCULATORWINDOW_H
#define CALCULATORWINDOW_H

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QMouseEvent>
#include <QPoint>
#include <QPushButton>
#include <QStackedWidget>

class MathEngine;
class ProgrammerEngine;
class CurrencyManager;

class CalculatorWindow: public QMainWindow
{
  Q_OBJECT

protected:
  void keyPressEvent(QKeyEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;

public:
  explicit CalculatorWindow(QWidget* parent = nullptr);
  ~CalculatorWindow();

private slots:
  void onMathButtonClicked();
  void onMathCalculate();
  void onMathClear();
  void onMathBackspace();

  void onTempConvert();
  void onNumConvert();

  void onCurrencyConvert();
  void onCurrencyFetchRates();
  void onCurrencyRatesUpdated();
  void onCurrencyError(const QString& msg);

  void onProgButtonClicked();
  void onProgCalculate();
  void onProgClear();
  void onProgBackspace();
  void onProgBaseChanged(int index);
  void onProgDisplayUpdated();

private:
  void setupUi();
  void updateMathUI();

  QPoint          dragPosition;
  QListWidget*    sidebarList;
  QStackedWidget* stackedWidget;

  MathEngine*       mathEngine;
  ProgrammerEngine* progEngine;
  CurrencyManager*  currencyManager;

  QLabel*             mathHistory;
  QLineEdit*          mathDisplay;
  QDoubleSpinBox*     tempInput;
  QComboBox*          tempFrom;
  QComboBox*          tempTo;
  QLabel*             tempResult;
  QLineEdit*          numInput;
  QComboBox*          numFrom;
  QComboBox*          numTo;
  QLabel*             numResult;
  QDoubleSpinBox*     currencyInput;
  QComboBox*          currencyFrom;
  QComboBox*          currencyTo;
  QLabel*             currencyResult;
  QLabel*             currencyStatus;
  QLabel*             progHistory;
  QLineEdit*          progDisplay;
  QComboBox*          progBaseCombo;
  QList<QPushButton*> progButtons;
};

#endif  // CALCULATORWINDOW_H
