#ifndef CALCULATORWINDOW_H
#define CALCULATORWINDOW_H

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMap>
#include <QMouseEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPoint>
#include <QPushButton>
#include <QTabWidget>

class CalculatorWindow : public QMainWindow
{
  Q_OBJECT

protected:
  void keyPressEvent(QKeyEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;

public:
  explicit CalculatorWindow(QWidget *parent = nullptr);
  ~CalculatorWindow();

private slots:
  // Math slots
  void onMathButtonClicked();
  void onMathCalculate();
  void onMathClear();

  // Temperature slots
  void onTempConvert();

  // Number Conversion slots
  void onNumConvert();
  void onMathBackspace();

  // Currency slots
  void onCurrencyConvert();
  void onCurrencyFetchRates();
  void onCurrencyNetworkReply(QNetworkReply *reply);

  // Programmer slots
  void onProgButtonClicked();
  void onProgCalculate();
  void onProgClear();
  void onProgBackspace();
  void onProgBaseChanged(int index);

private:
  void setupUi();
  void processMathInput(const QString &text);
  void processProgInput(const QString &text);

  QPoint dragPosition;
  QTabWidget *tabWidget;

  // Math UI
  QLabel *mathHistory;
  QLineEdit *mathDisplay;
  double currentMathValue;
  QString pendingMathOp;
  bool waitingForNewOperand;
  bool isUpdatingBoxes;
  int prevTempFromIdx;
  int prevTempToIdx;
  int prevNumFromIdx;
  int prevNumToIdx;
  int prevCurrFromIdx;
  int prevCurrToIdx;

  // Temperature UI
  QDoubleSpinBox *tempInput;
  QComboBox *tempFrom;
  QComboBox *tempTo;
  QLabel *tempResult;

  // Number Conversion UI
  QLineEdit *numInput;
  QComboBox *numFrom;
  QComboBox *numTo;
  QLabel *numResult;

  // Currency UI
  QDoubleSpinBox *currencyInput;
  QComboBox *currencyFrom;
  QComboBox *currencyTo;
  QLabel *currencyResult;
  QLabel *currencyStatus;
  QNetworkAccessManager *networkManager;
  QMap<QString, double> exchangeRates; // Maps currency code to its rate against USD

  // Programmer UI
  QLabel *progHistory;
  QLineEdit *progDisplay;
  QComboBox *progBaseCombo;
  qint64 currentProgValue;
  QString pendingProgOp;
  bool waitingForNewProgOperand;
  int currentProgBase;
  QList<QPushButton *> progButtons;
};

#endif // CALCULATORWINDOW_H
