#include "CalculatorWindow.h"
#include <QApplication>
#include <QClipboard>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QKeyEvent>
#include <QNetworkRequest>
#include <QScreen>
#include <QVBoxLayout>

CalculatorWindow::CalculatorWindow(QWidget *parent) :
    QMainWindow(parent), currentMathValue(0.0), pendingMathOp(""), waitingForNewOperand(true), isUpdatingBoxes(false), prevTempFromIdx(0),
    prevTempToIdx(1), prevNumFromIdx(0), prevNumToIdx(3), prevCurrFromIdx(0), prevCurrToIdx(1), currentProgValue(0), pendingProgOp(""),
    waitingForNewProgOperand(true), currentProgBase(10)
{
  setupUi();

  exchangeRates["USD"] = 1.0;
  exchangeRates["IDR"] = 17000.0;
  exchangeRates["EUR"] = 0.92;
  exchangeRates["GBP"] = 0.79;
  exchangeRates["JPY"] = 150.0;

  networkManager = new QNetworkAccessManager(this);
  connect(networkManager, &QNetworkAccessManager::finished, this, &CalculatorWindow::onCurrencyNetworkReply);
  onCurrencyFetchRates(); // fetch on startup
}

CalculatorWindow::~CalculatorWindow() {}

void CalculatorWindow::setupUi()
{
  setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
  setAttribute(Qt::WA_TranslucentBackground);
  setWindowIcon(QIcon(":/Images/Icon.ico"));
  setWindowTitle("Calculator");
  resize(400, 530);

  QFrame *mainFrame = new QFrame(this);
  mainFrame->setObjectName("mainFrame");
  mainFrame->setStyleSheet("QFrame#mainFrame { "
                           "background-color: #f0f0f0; "
                           "border-radius: 15px; "
                           "border: 1px solid #c0c0c0; "
                           "}");
  setCentralWidget(mainFrame);

  QVBoxLayout *mainLayout = new QVBoxLayout(mainFrame);
  mainLayout->setContentsMargins(15, 10, 15, 15);

  // Custom Title Bar
  QHBoxLayout *titleBarLayout = new QHBoxLayout();
  titleBarLayout->setContentsMargins(0, 0, 0, 10);
  QLabel *titleIcon = new QLabel();
  titleIcon->setPixmap(QIcon(":/Images/Icon.ico").pixmap(18, 18));
  QLabel *titleLabel = new QLabel("<b>Calculator</b>");
  titleLabel->setStyleSheet("color: #444; font-size: 13px;");

  QPushButton *closeBtn = new QPushButton("✕");
  closeBtn->setFixedSize(28, 28);
  closeBtn->setFocusPolicy(Qt::NoFocus);
  closeBtn->setStyleSheet("QPushButton { background-color: transparent; border: none; font-weight: "
                          "bold; font-size: 14px; color: #888; border-radius: 14px; }"
                          "QPushButton:hover { background-color: #ff4d4d; color: white; }");
  connect(closeBtn, &QPushButton::clicked, this, &QWidget::close);

  QPushButton *minBtn = new QPushButton("–");
  minBtn->setFixedSize(28, 28);
  minBtn->setFocusPolicy(Qt::NoFocus);
  minBtn->setStyleSheet("QPushButton { background-color: transparent; border: none; font-weight: "
                        "bold; font-size: 14px; color: #888; border-radius: 14px; }"
                        "QPushButton:hover { background-color: #dddddd; color: black; }");
  connect(minBtn, &QPushButton::clicked, this, &QWidget::showMinimized);

  titleBarLayout->addWidget(titleIcon);
  titleBarLayout->addWidget(titleLabel);
  titleBarLayout->addStretch();
  titleBarLayout->addWidget(minBtn);
  titleBarLayout->addWidget(closeBtn);

  mainLayout->addLayout(titleBarLayout);

  tabWidget = new QTabWidget(mainFrame);
  mainLayout->addWidget(tabWidget);

  // --- Math Tab ---
  QWidget *mathTab = new QWidget();
  QVBoxLayout *mathLayout = new QVBoxLayout(mathTab);

  mathHistory = new QLabel("");
  mathHistory->setAlignment(Qt::AlignRight);
  mathHistory->setStyleSheet("color: gray;");
  mathLayout->addWidget(mathHistory);

  mathDisplay = new QLineEdit("0");
  mathDisplay->setReadOnly(true);
  mathDisplay->setAlignment(Qt::AlignRight);
  mathDisplay->setFocusPolicy(Qt::NoFocus);
  QFont font = mathDisplay->font();
  font.setPointSize(24);
  mathDisplay->setFont(font);
  mathLayout->addWidget(mathDisplay);

  QGridLayout *gridLayout = new QGridLayout();
  QStringList buttons = { "7", "8", "9", "/", "4", "5", "6", "*", "1", "2", "3", "-", "0", ".", "C", "+" };
  int pos = 0;
  for (int i = 0; i < 4; ++i)
  {
    for (int j = 0; j < 4; ++j)
    {
      QPushButton *btn = new QPushButton(buttons[pos]);
      btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      btn->setFont(font);
      btn->setFocusPolicy(Qt::NoFocus);
      gridLayout->addWidget(btn, i, j);

      if (buttons[pos] == "C")
      {
        connect(btn, &QPushButton::clicked, this, &CalculatorWindow::onMathClear);
      }
      else
      {
        connect(btn, &QPushButton::clicked, this, &CalculatorWindow::onMathButtonClicked);
      }
      pos++;
    }
  }
  QPushButton *backBtn = new QPushButton("⌫");
  backBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  backBtn->setFont(font);
  backBtn->setFocusPolicy(Qt::NoFocus);
  connect(backBtn, &QPushButton::clicked, this, &CalculatorWindow::onMathBackspace);
  gridLayout->addWidget(backBtn, 4, 0, 1, 2);

  QPushButton *eqBtn = new QPushButton("=");
  eqBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  eqBtn->setFont(font);
  eqBtn->setFocusPolicy(Qt::NoFocus);
  connect(eqBtn, &QPushButton::clicked, this, &CalculatorWindow::onMathCalculate);
  gridLayout->addWidget(eqBtn, 4, 2, 1, 2);

  mathLayout->addLayout(gridLayout);
  tabWidget->addTab(mathTab, "Calculator");

  // --- Temperature Tab ---
  QWidget *tempTab = new QWidget();
  QVBoxLayout *tempLayout = new QVBoxLayout(tempTab);

  tempInput = new QDoubleSpinBox();
  tempInput->setRange(-10000, 10000);
  tempInput->setDecimals(2);
  tempInput->setValue(0);

  tempFrom = new QComboBox();
  tempFrom->addItems({ "Celsius", "Fahrenheit", "Kelvin" });

  tempTo = new QComboBox();
  tempTo->addItems({ "Celsius", "Fahrenheit", "Kelvin" });
  tempTo->setCurrentIndex(1);

  tempResult = new QLabel("Result: 0.00 Fahrenheit");
  tempResult->setTextInteractionFlags(Qt::TextSelectableByMouse);

  connect(tempInput, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CalculatorWindow::onTempConvert);
  connect(tempFrom, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CalculatorWindow::onTempConvert);
  connect(tempTo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CalculatorWindow::onTempConvert);

  tempLayout->addWidget(new QLabel("Value:"));
  tempLayout->addWidget(tempInput);
  tempLayout->addWidget(new QLabel("From:"));
  tempLayout->addWidget(tempFrom);
  tempLayout->addWidget(new QLabel("To:"));
  tempLayout->addWidget(tempTo);

  QHBoxLayout *tempResLayout = new QHBoxLayout();
  tempResLayout->addWidget(tempResult);
  QPushButton *copyTempBtn = new QPushButton();
  copyTempBtn->setIcon(QIcon(":/Images/CopyPaste.png"));
  copyTempBtn->setFixedSize(30, 30);
  connect(copyTempBtn, &QPushButton::clicked, this,
          [this]() { QApplication::clipboard()->setText(tempResult->text().remove("Result: ")); });
  tempResLayout->addWidget(copyTempBtn);
  tempResLayout->addStretch();
  tempLayout->addLayout(tempResLayout);

  tempLayout->addStretch();
  tabWidget->addTab(tempTab, "Temperature");

  // --- Number Conversion Tab ---
  QWidget *numTab = new QWidget();
  QVBoxLayout *numLayout = new QVBoxLayout(numTab);

  numInput = new QLineEdit();

  numFrom = new QComboBox();
  numFrom->addItems({ "Decimal", "Binary", "Octal", "Hexadecimal", "Text" });

  numTo = new QComboBox();
  numTo->addItems({ "Decimal", "Binary", "Octal", "Hexadecimal", "Text" });
  numTo->setCurrentIndex(3);

  numResult = new QLabel("Result: ");
  numResult->setTextInteractionFlags(Qt::TextSelectableByMouse);

  connect(numInput, &QLineEdit::textChanged, this, &CalculatorWindow::onNumConvert);
  connect(numFrom, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CalculatorWindow::onNumConvert);
  connect(numTo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CalculatorWindow::onNumConvert);

  numLayout->addWidget(new QLabel("Value:"));
  numLayout->addWidget(numInput);
  numLayout->addWidget(new QLabel("From:"));
  numLayout->addWidget(numFrom);
  numLayout->addWidget(new QLabel("To:"));
  numLayout->addWidget(numTo);

  QHBoxLayout *numResLayout = new QHBoxLayout();
  numResLayout->addWidget(numResult);
  QPushButton *copyNumBtn = new QPushButton();
  copyNumBtn->setIcon(QIcon(":/Images/CopyPaste.png"));
  copyNumBtn->setFixedSize(30, 30);
  connect(copyNumBtn, &QPushButton::clicked, this, [this]() { QApplication::clipboard()->setText(numResult->text().remove("Result: ")); });
  numResLayout->addWidget(copyNumBtn);
  numResLayout->addStretch();
  numLayout->addLayout(numResLayout);

  numLayout->addStretch();
  tabWidget->addTab(numTab, "Number");

  // --- Currency Tab ---
  QWidget *currTab = new QWidget();
  QVBoxLayout *currLayout = new QVBoxLayout(currTab);

  currencyInput = new QDoubleSpinBox();
  currencyInput->setRange(0, 1e9);
  currencyInput->setDecimals(2);
  currencyInput->setValue(1);

  currencyFrom = new QComboBox();
  currencyFrom->addItems(exchangeRates.keys());

  currencyTo = new QComboBox();
  currencyTo->addItems(exchangeRates.keys());
  currencyTo->setCurrentText("IDR");

  currencyResult = new QLabel("Result: 17000.00 IDR");
  currencyResult->setTextInteractionFlags(Qt::TextSelectableByMouse);
  currencyStatus = new QLabel("Rates: Offline (Using Fallback 1 USD = 17000 IDR)");

  QPushButton *fetchBtn = new QPushButton("Fetch Real-time Rates");
  connect(fetchBtn, &QPushButton::clicked, this, &CalculatorWindow::onCurrencyFetchRates);

  connect(currencyInput, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &CalculatorWindow::onCurrencyConvert);
  connect(currencyFrom, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CalculatorWindow::onCurrencyConvert);
  connect(currencyTo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CalculatorWindow::onCurrencyConvert);

  currLayout->addWidget(new QLabel("Amount:"));
  currLayout->addWidget(currencyInput);
  currLayout->addWidget(new QLabel("From:"));
  currLayout->addWidget(currencyFrom);
  currLayout->addWidget(new QLabel("To:"));
  currLayout->addWidget(currencyTo);

  QHBoxLayout *currResLayout = new QHBoxLayout();
  currResLayout->addWidget(currencyResult);
  QPushButton *copyCurrBtn = new QPushButton();
  copyCurrBtn->setIcon(QIcon(":/Images/CopyPaste.png"));
  copyCurrBtn->setFixedSize(30, 30);
  connect(copyCurrBtn, &QPushButton::clicked, this,
          [this]() { QApplication::clipboard()->setText(currencyResult->text().remove("Result: ")); });
  currResLayout->addWidget(copyCurrBtn);
  currResLayout->addStretch();
  currLayout->addLayout(currResLayout);

  currLayout->addWidget(fetchBtn);
  currLayout->addWidget(currencyStatus);
  currLayout->addStretch();
  tabWidget->addTab(currTab, "Currency");

  // --- Programmer Tab ---
  QWidget *progTab = new QWidget();
  QVBoxLayout *progLayout = new QVBoxLayout(progTab);

  progHistory = new QLabel("");
  progHistory->setAlignment(Qt::AlignRight);
  progHistory->setStyleSheet("color: gray;");
  progLayout->addWidget(progHistory);

  progDisplay = new QLineEdit("0");
  progDisplay->setReadOnly(true);
  progDisplay->setAlignment(Qt::AlignRight);
  progDisplay->setFocusPolicy(Qt::NoFocus);
  progDisplay->setFont(font);
  progLayout->addWidget(progDisplay);

  progBaseCombo = new QComboBox();
  progBaseCombo->addItems({ "Hexadecimal", "Decimal", "Octal", "Binary" });
  progBaseCombo->setCurrentIndex(1); // Decimal
  connect(progBaseCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CalculatorWindow::onProgBaseChanged);
  progLayout->addWidget(progBaseCombo);

  QGridLayout *progGrid = new QGridLayout();
  QStringList progBtnLabels = { "A", "B", "C", "D", "E", "F", "Clr", "⌫", "7", "8", "9", "/",
                                "4", "5", "6", "*", "1", "2", "3",   "-", "0", "",  "=", "+" };

  int pPos = 0;
  for (int i = 0; i < 6; ++i)
  {
    for (int j = 0; j < 4; ++j)
    {
      if (progBtnLabels[pPos] == "")
      {
        pPos++;
        continue;
      }
      QPushButton *btn = new QPushButton(progBtnLabels[pPos]);
      btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      btn->setFont(font);
      btn->setFocusPolicy(Qt::NoFocus);

      if (progBtnLabels[pPos] == "0")
      {
        progGrid->addWidget(btn, i, j, 1, 2);
        j++;
      }
      else
      {
        progGrid->addWidget(btn, i, j);
      }

      if (progBtnLabels[pPos] == "Clr")
      {
        connect(btn, &QPushButton::clicked, this, &CalculatorWindow::onProgClear);
      }
      else if (progBtnLabels[pPos] == "⌫")
      {
        connect(btn, &QPushButton::clicked, this, &CalculatorWindow::onProgBackspace);
      }
      else if (progBtnLabels[pPos] == "=")
      {
        connect(btn, &QPushButton::clicked, this, &CalculatorWindow::onProgCalculate);
      }
      else
      {
        connect(btn, &QPushButton::clicked, this, &CalculatorWindow::onProgButtonClicked);
        if ((progBtnLabels[pPos] >= "0" && progBtnLabels[pPos] <= "9") || (progBtnLabels[pPos] >= "A" && progBtnLabels[pPos] <= "F"))
        {
          progButtons.append(btn);
        }
      }
      pPos++;
    }
  }
  progLayout->addLayout(progGrid);
  tabWidget->addTab(progTab, "Programmer");

  // mulai konversi
  onTempConvert();
  onCurrencyConvert();
}

void CalculatorWindow::onMathButtonClicked()
{
  QPushButton *btn = qobject_cast<QPushButton *>(sender());
  if (!btn)
    return;
  processMathInput(btn->text());
}

void CalculatorWindow::processMathInput(const QString &text)
{
  if (QString("/ * - +").contains(text))
  {
    if (!waitingForNewOperand)
    {
      onMathCalculate();
    }
    pendingMathOp = text;
    currentMathValue = mathDisplay->text().toDouble();
    waitingForNewOperand = true;
    mathDisplay->setText("0"); // clear

    mathHistory->setText(QString::number(currentMathValue, 'g', 15) + " " + pendingMathOp);
  }
  else
  {
    if (waitingForNewOperand)
    {
      mathDisplay->clear();
      waitingForNewOperand = false;
    }
    if (text == "." && mathDisplay->text().contains("."))
    {
      return;
    }
    mathDisplay->setText(mathDisplay->text() + text);
  }
}

void CalculatorWindow::keyPressEvent(QKeyEvent *event)
{
  int tabIdx = tabWidget->currentIndex();
  if (tabIdx != 0 && tabIdx != 4)
  {
    QMainWindow::keyPressEvent(event);
    return;
  }

  QString text = event->text();
  int key = event->key();

  if (key == Qt::Key_Enter || key == Qt::Key_Return || key == Qt::Key_Equal)
  {
    if (tabIdx == 0)
      onMathCalculate();
    else
      onProgCalculate();
  }
  else if (key == Qt::Key_Escape)
  {
    if (tabIdx == 0)
      onMathClear();
    else
      onProgClear();
  }
  else if (key == Qt::Key_Backspace || key == Qt::Key_Delete || text == "\b" || text == "\x7F")
  {
    if (tabIdx == 0)
      onMathBackspace();
    else
      onProgBackspace();
  }
  else if (!text.isEmpty())
  {
    if (tabIdx == 0 && QString("0123456789.+-*/").contains(text))
    {
      processMathInput(text);
    }
    else if (tabIdx == 4)
    {
      text = text.toUpper();
      bool valid = false;
      if (QString("+-*/").contains(text))
      {
        valid = true;
      }
      else if (currentProgBase == 16 && QString("0123456789ABCDEF").contains(text))
      {
        valid = true;
      }
      else if (currentProgBase == 10 && QString("0123456789").contains(text))
      {
        valid = true;
      }
      else if (currentProgBase == 8 && QString("01234567").contains(text))
      {
        valid = true;
      }
      else if (currentProgBase == 2 && QString("01").contains(text))
      {
        valid = true;
      }
      if (valid)
      {
        processProgInput(text);
      }
      else
      {
        QMainWindow::keyPressEvent(event);
      }
    }
    else
    {
      QMainWindow::keyPressEvent(event);
    }
  }
  else
  {
    QMainWindow::keyPressEvent(event);
  }
}

void CalculatorWindow::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
  {
    dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
    event->accept();
  }
}

void CalculatorWindow::mouseMoveEvent(QMouseEvent *event)
{
  if (event->buttons() & Qt::LeftButton)
  {
    move(event->globalPosition().toPoint() - dragPosition);
    event->accept();
  }
}

void CalculatorWindow::onMathCalculate()
{
  if (pendingMathOp.isEmpty())
    return;
  double operand = mathDisplay->text().toDouble();
  double result = 0.0;
  if (pendingMathOp == "+")
    result = currentMathValue + operand;
  else if (pendingMathOp == "-")
    result = currentMathValue - operand;
  else if (pendingMathOp == "*")
    result = currentMathValue * operand;
  else if (pendingMathOp == "/")
  {
    if (operand == 0.0)
    {
      mathDisplay->setText("Error");
      pendingMathOp = "";
      waitingForNewOperand = true;
      return;
    }
    result = currentMathValue / operand;
  }
  // 15 digit angka batas toleransi sebelum akhirnya mengeluarkan "e".
  mathDisplay->setText(QString::number(result, 'g', 15));
  mathHistory->setText("");
  pendingMathOp = "";
  waitingForNewOperand = true;
  currentMathValue = result;
}

void CalculatorWindow::onMathClear()
{
  mathDisplay->setText("0");
  mathHistory->setText("");
  pendingMathOp = "";
  currentMathValue = 0.0;
  waitingForNewOperand = true;
}

void CalculatorWindow::onMathBackspace()
{
  if (!waitingForNewOperand)
  {
    QString currentText = mathDisplay->text();
    if (currentText.length() > 0)
    {
      currentText.chop(1);
    }
    if (currentText.isEmpty() || currentText == "-")
    {
      currentText = "0";
      waitingForNewOperand = true;
    }
    mathDisplay->setText(currentText);
  }
}

void CalculatorWindow::onTempConvert()
{
  if (isUpdatingBoxes)
    return;

  if (tempFrom->currentIndex() == tempTo->currentIndex())
  {
    isUpdatingBoxes = true;
    if (tempFrom->currentIndex() != prevTempFromIdx)
    {
      tempTo->setCurrentIndex(prevTempFromIdx);
    }
    else if (tempTo->currentIndex() != prevTempToIdx)
    {
      tempFrom->setCurrentIndex(prevTempToIdx);
    }
    isUpdatingBoxes = false;
  }
  prevTempFromIdx = tempFrom->currentIndex();
  prevTempToIdx = tempTo->currentIndex();

  double val = tempInput->value();
  QString from = tempFrom->currentText();
  QString to = tempTo->currentText();

  // ubah ke celsius dulu
  double c = val;
  if (from == "Fahrenheit")
    c = (val - 32.0) * 5.0 / 9.0;
  else if (from == "Kelvin")
    c = val - 273.15;

  // ubah celsius ke target
  double result = c;
  if (to == "Fahrenheit")
    result = c * 9.0 / 5.0 + 32.0;
  else if (to == "Kelvin")
    result = c + 273.15;

  tempResult->setText("Result: " + QString::number(result, 'f', 2) + " " + to);
}

void CalculatorWindow::onCurrencyConvert()
{
  if (isUpdatingBoxes)
    return;

  if (currencyFrom->currentIndex() == currencyTo->currentIndex())
  {
    isUpdatingBoxes = true;
    if (currencyFrom->currentIndex() != prevCurrFromIdx)
    {
      currencyTo->setCurrentIndex(prevCurrFromIdx);
    }
    else if (currencyTo->currentIndex() != prevCurrToIdx)
    {
      currencyFrom->setCurrentIndex(prevCurrToIdx);
    }
    isUpdatingBoxes = false;
  }
  prevCurrFromIdx = currencyFrom->currentIndex();
  prevCurrToIdx = currencyTo->currentIndex();

  double val = currencyInput->value();
  QString from = currencyFrom->currentText();
  QString to = currencyTo->currentText();

  double result = val;
  if (exchangeRates.contains(from) && exchangeRates.contains(to))
  {
    result = (val / exchangeRates[from]) * exchangeRates[to];
  }

  currencyResult->setText("Result: " + QString::number(result, 'f', 2) + " " + to);
}

void CalculatorWindow::onNumConvert()
{
  if (isUpdatingBoxes)
    return;

  if (numFrom->currentIndex() == numTo->currentIndex())
  {
    isUpdatingBoxes = true;
    if (numFrom->currentIndex() != prevNumFromIdx)
    {
      numTo->setCurrentIndex(prevNumFromIdx);
    }
    else if (numTo->currentIndex() != prevNumToIdx)
    {
      numFrom->setCurrentIndex(prevNumToIdx);
    }
    isUpdatingBoxes = false;
  }
  prevNumFromIdx = numFrom->currentIndex();
  prevNumToIdx = numTo->currentIndex();

  QString valStr = numInput->text();
  if (valStr.isEmpty())
  {
    numResult->setText("Result: ");
    return;
  }

  QString from = numFrom->currentText();
  QString to = numTo->currentText();

  qulonglong num = 0;
  bool ok = true;

  if (from == "Text")
  {
    QByteArray bytes = valStr.toUtf8();
    for (int i = 0; i < qMin(bytes.size(), 8); ++i)
    {
      num = (num << 8) | (quint8)bytes[i];
    }
  }
  else
  {
    if (from == "Decimal")
      num = valStr.toULongLong(&ok, 10);
    else if (from == "Binary")
      num = valStr.toULongLong(&ok, 2);
    else if (from == "Octal")
      num = valStr.toULongLong(&ok, 8);
    else if (from == "Hexadecimal")
      num = valStr.toULongLong(&ok, 16);
  }

  if (!ok)
  {
    numResult->setText("Result: Invalid Input");
    return;
  }

  QString resStr;
  if (to == "Text")
  {
    QByteArray bytes;
    qulonglong temp = num;
    while (temp > 0)
    {
      bytes.prepend((char)(temp & 0xFF));
      temp >>= 8;
    }
    resStr = QString::fromUtf8(bytes);
  }
  else
  {
    if (to == "Decimal")
      resStr = QString::number(num, 10);
    else if (to == "Binary")
      resStr = QString::number(num, 2);
    else if (to == "Octal")
      resStr = QString::number(num, 8);
    else if (to == "Hexadecimal")
      resStr = QString::number(num, 16).toUpper();
  }

  numResult->setText("Result: " + resStr);
}

void CalculatorWindow::onCurrencyFetchRates()
{
  currencyStatus->setText("Fetching rates...");
  networkManager->get(QNetworkRequest(QUrl("https://api.exchangerate-api.com/v4/latest/USD")));
}

void CalculatorWindow::onCurrencyNetworkReply(QNetworkReply *reply)
{
  if (reply->error() == QNetworkReply::NoError)
  {
    QByteArray response = reply->readAll();
    QJsonDocument json = QJsonDocument::fromJson(response);
    QJsonObject obj = json.object();
    if (obj.contains("rates"))
    {
      QJsonObject rates = obj["rates"].toObject();

      QString currentFrom = currencyFrom->currentText();
      QString currentTo = currencyTo->currentText();

      currencyFrom->blockSignals(true);
      currencyTo->blockSignals(true);

      currencyFrom->clear();
      currencyTo->clear();

      QStringList keys = rates.keys();
      currencyFrom->addItems(keys);
      currencyTo->addItems(keys);

      exchangeRates.clear();
      for (const QString &key : keys)
      {
        exchangeRates[key] = rates[key].toDouble();
      }

      int fromIdx = currencyFrom->findText(currentFrom);
      if (fromIdx >= 0)
        currencyFrom->setCurrentIndex(fromIdx);
      int toIdx = currencyTo->findText(currentTo);
      if (toIdx >= 0)
        currencyTo->setCurrentIndex(toIdx);

      currencyFrom->blockSignals(false);
      currencyTo->blockSignals(false);

      currencyStatus->setText("Rates: Online, Real-time rates fetched");
      onCurrencyConvert(); // trigger update
    }
  }
  else
  {
    currencyStatus->setText("Rates: Failed to fetch. Using fallback.");
  }
  reply->deleteLater();
}

void CalculatorWindow::onProgButtonClicked()
{
  QPushButton *btn = qobject_cast<QPushButton *>(sender());
  if (!btn)
    return;
  processProgInput(btn->text());
}

void CalculatorWindow::processProgInput(const QString &text)
{
  if (QString("/ * - +").contains(text))
  {
    if (!waitingForNewProgOperand)
    {
      onProgCalculate();
    }
    pendingProgOp = text;

    currentProgValue = progDisplay->text().toLongLong(nullptr, currentProgBase);

    waitingForNewProgOperand = true;
    progDisplay->setText("0");
    progHistory->setText(QString::number(currentProgValue, currentProgBase).toUpper() + " " + pendingProgOp);
  }
  else
  {
    if (waitingForNewProgOperand)
    {
      progDisplay->clear();
      waitingForNewProgOperand = false;
    }
    progDisplay->setText(progDisplay->text() + text);
  }
}

void CalculatorWindow::onProgCalculate()
{
  if (pendingProgOp.isEmpty())
    return;

  qint64 operand = progDisplay->text().toLongLong(nullptr, currentProgBase);
  qint64 result = 0;

  if (pendingProgOp == "+")
    result = currentProgValue + operand;
  else if (pendingProgOp == "-")
    result = currentProgValue - operand;
  else if (pendingProgOp == "*")
    result = currentProgValue * operand;
  else if (pendingProgOp == "/")
  {
    if (operand == 0)
    {
      progDisplay->setText("Error");
      pendingProgOp = "";
      waitingForNewProgOperand = true;
      return;
    }
    result = currentProgValue / operand;
  }

  progDisplay->setText(QString::number(result, currentProgBase).toUpper());
  progHistory->setText("");
  pendingProgOp = "";
  waitingForNewProgOperand = true;
  currentProgValue = result;
}

void CalculatorWindow::onProgClear()
{
  progDisplay->setText("0");
  progHistory->setText("");
  pendingProgOp = "";
  currentProgValue = 0;
  waitingForNewProgOperand = true;
}

void CalculatorWindow::onProgBackspace()
{
  if (!waitingForNewProgOperand)
  {
    QString currentText = progDisplay->text();
    if (currentText.length() > 0)
    {
      currentText.chop(1);
    }
    if (currentText.isEmpty() || currentText == "-")
    {
      currentText = "0";
      waitingForNewProgOperand = true;
    }
    progDisplay->setText(currentText);
  }
}

void CalculatorWindow::onProgBaseChanged(int index)
{
  int newBase = 10;
  if (index == 0)
    newBase = 16;
  else if (index == 2)
    newBase = 8;
  else if (index == 3)
    newBase = 2;

  QString currentText = progDisplay->text();
  if (currentText != "Error")
  {
    qint64 val = currentText.toLongLong(nullptr, currentProgBase);
    progDisplay->setText(QString::number(val, newBase).toUpper());
  }

  if (!progHistory->text().isEmpty())
  {
    QString histText = progHistory->text();
    // Rebuild history string
    progHistory->setText(QString::number(currentProgValue, newBase).toUpper() + " " + pendingProgOp);
  }

  currentProgBase = newBase;

  for (QPushButton *btn : progButtons)
  {
    QString t = btn->text();
    bool enable = false;
    if (index == 0)
    { // Hex
      enable = true;
    }
    else if (index == 1)
    { // Dec
      if (t >= "0" && t <= "9")
        enable = true;
    }
    else if (index == 2)
    { // Oct
      if (t >= "0" && t <= "7")
        enable = true;
    }
    else if (index == 3)
    { // Bin
      if (t == "0" || t == "1")
        enable = true;
    }
    btn->setEnabled(enable);
  }
}
