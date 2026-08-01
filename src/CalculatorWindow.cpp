#include "CalculatorWindow.h"

#include "ConversionEngine.h"
#include "CurrencyManager.h"
#include "MathEngine.h"
#include "ProgrammerEngine.h"

#include <QApplication>
#include <QClipboard>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIcon>
#include <QKeyEvent>
#include <QScreen>
#include <QStyledItemDelegate>
#include <QVBoxLayout>

CalculatorWindow::CalculatorWindow(QWidget* parent) :
    QMainWindow(parent)
{

  mathEngine      = new MathEngine();
  progEngine      = new ProgrammerEngine(this);
  currencyManager = new CurrencyManager(this);

  connect(
    progEngine, &ProgrammerEngine::displayUpdated, this, &CalculatorWindow::onProgDisplayUpdated
  );
  connect(
    currencyManager, &CurrencyManager::ratesUpdated, this, &CalculatorWindow::onCurrencyRatesUpdated
  );
  connect(
    currencyManager, &CurrencyManager::errorOccurred, this, &CalculatorWindow::onCurrencyError
  );

  setupUi();

  currencyManager->fetchRates();  // fetch on startup
}

CalculatorWindow::~CalculatorWindow() { delete mathEngine; }

void CalculatorWindow::setupUi()
{
  setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
  setAttribute(Qt::WA_TranslucentBackground);
  setWindowIcon(QIcon(":/images/Icon.ico"));
  setWindowTitle("Calculator");
  resize(750, 500);

  this->setStyleSheet(R"(
    QWidget {
      font-family: 'Segoe UI', 'Helvetica Neue', sans-serif;
      color: #e0e0e0;
    }
    QFrame#mainFrame {
      background-color: #202020;
      border-radius: 12px;
      border: 1px solid #3d3d3d;
    }
    QListWidget {
      background-color: transparent;
      border: none;
      outline: 0;
    }
    QListWidget::item {
      padding: 10px;
      margin: 2px 10px;
      border-radius: 6px;
    }
    QListWidget::item:hover {
      background-color: #333333;
    }
    QListWidget::item:selected {
      background-color: #404040;
      color: #ffffff;
      font-weight: bold;
    }
    QLineEdit, QDoubleSpinBox, QComboBox {
      background-color: #2b2b2b;
      border: 1px solid #3d3d3d;
      border-radius: 6px;
      padding: 5px 10px;
      color: #ffffff;
    }
    QLineEdit:focus, QDoubleSpinBox:focus, QComboBox:focus {
      border: 1px solid #0078D7;
    }
    QComboBox::drop-down {
      border: none;
    }
    QComboBox QAbstractItemView {
      background-color: #2b2b2b;
      border: 1px solid #3d3d3d;
      selection-background-color: #404040;
      color: #ffffff;
      outline: 0px;
    }
    QComboBox QAbstractItemView::viewport {
      background-color: #2b2b2b;
    }
    QComboBox QAbstractItemView::item {
      background-color: #2b2b2b;
      color: #ffffff;
      padding: 4px;
    }
    QComboBox QAbstractItemView::item:hover {
      background-color: #404040;
    }
    QListView { 
      background-color: #2b2b2b; 
    }
    QComboBoxPrivateContainer {
      background-color: #2b2b2b;
    }
    QPushButton {
      background-color: #333333;
      border: 1px solid #404040;
      border-radius: 8px;
      padding: 10px;
      font-size: 14px;
    }
    QPushButton:hover {
      background-color: #3e3e3e;
      border: 1px solid #505050;
    }
    QPushButton:pressed {
      background-color: #252525;
    }
    QPushButton#mathDigitBtn {
      background-color: #2b2b2b;
    }
    QPushButton#mathDigitBtn:hover {
      background-color: #363636;
    }
    QPushButton#mathOpBtn {
      background-color: #333333;
    }
    QPushButton#mathOpBtn:hover {
      background-color: #404040;
    }
    QPushButton#mathEqBtn {
      background-color: #0067b8;
      color: white;
    }
    QPushButton#mathEqBtn:hover {
      background-color: #005a9e;
    }
  )");

  QFrame* mainFrame = new QFrame(this);
  mainFrame->setObjectName("mainFrame");
  setCentralWidget(mainFrame);

  QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
  shadow->setBlurRadius(20);
  shadow->setColor(QColor(0, 0, 0, 160));
  shadow->setOffset(0, 4);
  mainFrame->setGraphicsEffect(shadow);

  QVBoxLayout* mainLayout = new QVBoxLayout(mainFrame);
  mainLayout->setContentsMargins(10, 10, 10, 10);
  mainLayout->setSpacing(0);

  QHBoxLayout* titleBarLayout = new QHBoxLayout();
  titleBarLayout->setContentsMargins(10, 5, 5, 10);
  QLabel* titleIcon = new QLabel();
  titleIcon->setPixmap(QIcon(":/images/Icon.ico").pixmap(18, 18));
  QLabel* titleLabel = new QLabel("<b>Calculator</b>");
  titleLabel->setStyleSheet("color: #b0b0b0; font-size: 13px;");

  QPushButton* minBtn = new QPushButton(QString::fromUtf8("–"));
  minBtn->setFixedSize(30, 30);
  minBtn->setFocusPolicy(Qt::NoFocus);
  minBtn->setStyleSheet(
    "QPushButton { background-color: transparent; border: none; font-weight: "
    "bold; font-size: 14px; color: #b0b0b0; border-radius: 15px; }"
    "QPushButton:hover { background-color: #3d3d3d; color: white; }"
  );
  connect(minBtn, &QPushButton::clicked, this, &QWidget::showMinimized);

  QPushButton* closeBtn = new QPushButton(QString::fromUtf8("✕"));
  closeBtn->setFixedSize(30, 30);
  closeBtn->setFocusPolicy(Qt::NoFocus);
  closeBtn->setStyleSheet(
    "QPushButton { background-color: transparent; border: none; font-weight: "
    "bold; font-size: 14px; color: #b0b0b0; border-radius: 15px; }"
    "QPushButton:hover { background-color: #c42b1c; color: white; }"
  );
  connect(closeBtn, &QPushButton::clicked, this, &QWidget::close);

  titleBarLayout->addWidget(titleIcon);
  titleBarLayout->addSpacing(5);
  titleBarLayout->addWidget(titleLabel);
  titleBarLayout->addStretch();
  titleBarLayout->addWidget(minBtn);
  titleBarLayout->addWidget(closeBtn);

  mainLayout->addLayout(titleBarLayout);

  QHBoxLayout* bodyLayout = new QHBoxLayout();
  bodyLayout->setContentsMargins(0, 0, 0, 0);
  bodyLayout->setSpacing(10);
  mainLayout->addLayout(bodyLayout);

  sidebarList = new QListWidget(mainFrame);
  sidebarList->setFixedWidth(210);
  sidebarList->setFocusPolicy(Qt::NoFocus);
  sidebarList->addItem(new QListWidgetItem(QString::fromUtf8("Calculator")));
  sidebarList->addItem(new QListWidgetItem(QString::fromUtf8("Temperature")));
  sidebarList->addItem(new QListWidgetItem(QString::fromUtf8("Number")));
  sidebarList->addItem(new QListWidgetItem(QString::fromUtf8("Currency")));
  sidebarList->addItem(new QListWidgetItem(QString::fromUtf8("Programmer")));
  sidebarList->setCurrentRow(0);
  bodyLayout->addWidget(sidebarList);

  stackedWidget = new QStackedWidget(mainFrame);
  bodyLayout->addWidget(stackedWidget, 1);

  connect(
    sidebarList, &QListWidget::currentRowChanged, stackedWidget, &QStackedWidget::setCurrentIndex
  );

  // --- Math Tab ---
  QWidget*     mathTab    = new QWidget();
  QVBoxLayout* mathLayout = new QVBoxLayout(mathTab);
  mathLayout->setContentsMargins(10, 0, 10, 10);

  mathHistory = new QLabel("");
  mathHistory->setAlignment(Qt::AlignRight);
  mathHistory->setStyleSheet("color: #888888; font-size: 14px;");
  mathLayout->addWidget(mathHistory);

  mathDisplay = new QLineEdit("0");
  mathDisplay->setReadOnly(true);
  mathDisplay->setAlignment(Qt::AlignRight);
  mathDisplay->setFocusPolicy(Qt::NoFocus);
  mathDisplay->setStyleSheet(
    "background-color: transparent; border: none; font-size: 40px; "
    "font-weight: bold; margin-bottom: 10px;"
  );
  mathLayout->addWidget(mathDisplay);

  QGridLayout* gridLayout = new QGridLayout();
  gridLayout->setSpacing(8);
  QStringList buttons = {"7", "8", "9", "/", "4", "5", "6", "*",
                         "1", "2", "3", "-", "0", ".", "C", "+"};
  int         pos     = 0;
  QFont       mathBtnFont;
  mathBtnFont.setPointSize(16);
  mathBtnFont.setWeight(QFont::Medium);

  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      QPushButton* btn = new QPushButton(buttons[pos]);
      btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      btn->setFont(mathBtnFont);
      btn->setFocusPolicy(Qt::NoFocus);

      if (QString("0123456789.").contains(buttons[pos])) {
        btn->setObjectName("mathDigitBtn");
      }
      else {
        btn->setObjectName("mathOpBtn");
      }

      gridLayout->addWidget(btn, i, j);

      if (buttons[pos] == "C") {
        connect(btn, &QPushButton::clicked, this, &CalculatorWindow::onMathClear);
      }
      else {
        connect(btn, &QPushButton::clicked, this, &CalculatorWindow::onMathButtonClicked);
      }
      pos++;
    }
  }
  QPushButton* backBtn = new QPushButton(QString::fromUtf8("⌫"));
  backBtn->setObjectName("mathOpBtn");
  backBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  backBtn->setFont(mathBtnFont);
  backBtn->setFocusPolicy(Qt::NoFocus);
  connect(backBtn, &QPushButton::clicked, this, &CalculatorWindow::onMathBackspace);
  gridLayout->addWidget(backBtn, 4, 0, 1, 2);

  QPushButton* eqBtn = new QPushButton("=");
  eqBtn->setObjectName("mathEqBtn");
  eqBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  eqBtn->setFont(mathBtnFont);
  eqBtn->setFocusPolicy(Qt::NoFocus);
  connect(eqBtn, &QPushButton::clicked, this, &CalculatorWindow::onMathCalculate);
  gridLayout->addWidget(eqBtn, 4, 2, 1, 2);

  mathLayout->addLayout(gridLayout);
  stackedWidget->addWidget(mathTab);

  // --- Temperature Tab ---
  QWidget*     tempTab    = new QWidget();
  QVBoxLayout* tempLayout = new QVBoxLayout(tempTab);
  tempLayout->setContentsMargins(10, 20, 20, 20);

  tempInput = new QDoubleSpinBox();
  tempInput->setRange(-10000, 10000);
  tempInput->setDecimals(2);
  tempInput->setValue(0);
  tempInput->setFixedHeight(35);

  tempFrom = new QComboBox();
  tempFrom->addItems({"Celsius", "Fahrenheit", "Kelvin"});
  tempFrom->setFixedHeight(35);

  tempTo = new QComboBox();
  tempTo->addItems({"Celsius", "Fahrenheit", "Kelvin"});
  tempTo->setCurrentIndex(1);
  tempTo->setFixedHeight(35);

  tempResult = new QLabel("Result: 0.00 Fahrenheit");
  tempResult->setTextInteractionFlags(Qt::TextSelectableByMouse);
  tempResult->setStyleSheet("font-size: 16px; font-weight: bold; color: #ffffff;");

  connect(
    tempInput, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
    &CalculatorWindow::onTempConvert
  );
  connect(
    tempFrom, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
    &CalculatorWindow::onTempConvert
  );
  connect(
    tempTo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
    &CalculatorWindow::onTempConvert
  );

  tempLayout->addWidget(new QLabel("Value:"));
  tempLayout->addWidget(tempInput);
  tempLayout->addSpacing(10);
  tempLayout->addWidget(new QLabel("From:"));
  tempLayout->addWidget(tempFrom);
  tempLayout->addSpacing(10);
  tempLayout->addWidget(new QLabel("To:"));
  tempLayout->addWidget(tempTo);
  tempLayout->addSpacing(20);

  QHBoxLayout* tempResLayout = new QHBoxLayout();
  tempResLayout->addWidget(tempResult);
  QPushButton* copyTempBtn = new QPushButton("Copy");
  copyTempBtn->setFixedSize(80, 35);
  connect(copyTempBtn, &QPushButton::clicked, this, [this]() {
    QApplication::clipboard()->setText(tempResult->text().remove("Result: "));
  });
  tempResLayout->addWidget(copyTempBtn);
  tempResLayout->addStretch();
  tempLayout->addLayout(tempResLayout);
  tempLayout->addStretch();
  stackedWidget->addWidget(tempTab);

  // --- Number Conversion Tab ---
  QWidget*     numTab    = new QWidget();
  QVBoxLayout* numLayout = new QVBoxLayout(numTab);
  numLayout->setContentsMargins(10, 20, 20, 20);

  numInput = new QLineEdit();
  numInput->setFixedHeight(35);

  numFrom = new QComboBox();
  numFrom->addItems({"Decimal", "Binary", "Octal", "Hexadecimal", "Text"});
  numFrom->setFixedHeight(35);

  numTo = new QComboBox();
  numTo->addItems({"Decimal", "Binary", "Octal", "Hexadecimal", "Text"});
  numTo->setCurrentIndex(3);
  numTo->setFixedHeight(35);

  numResult = new QLabel("Result: ");
  numResult->setTextInteractionFlags(Qt::TextSelectableByMouse);
  numResult->setStyleSheet("font-size: 16px; font-weight: bold; color: #ffffff;");

  connect(numInput, &QLineEdit::textChanged, this, &CalculatorWindow::onNumConvert);
  connect(
    numFrom, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
    &CalculatorWindow::onNumConvert
  );
  connect(
    numTo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
    &CalculatorWindow::onNumConvert
  );

  numLayout->addWidget(new QLabel("Value:"));
  numLayout->addWidget(numInput);
  numLayout->addSpacing(10);
  numLayout->addWidget(new QLabel("From:"));
  numLayout->addWidget(numFrom);
  numLayout->addSpacing(10);
  numLayout->addWidget(new QLabel("To:"));
  numLayout->addWidget(numTo);
  numLayout->addSpacing(20);

  QHBoxLayout* numResLayout = new QHBoxLayout();
  numResLayout->addWidget(numResult);
  QPushButton* copyNumBtn = new QPushButton("Copy");
  copyNumBtn->setFixedSize(80, 35);
  connect(copyNumBtn, &QPushButton::clicked, this, [this]() {
    QApplication::clipboard()->setText(numResult->text().remove("Result: "));
  });
  numResLayout->addWidget(copyNumBtn);
  numResLayout->addStretch();
  numLayout->addLayout(numResLayout);
  numLayout->addStretch();
  stackedWidget->addWidget(numTab);

  // --- Currency Tab ---
  QWidget*     currTab    = new QWidget();
  QVBoxLayout* currLayout = new QVBoxLayout(currTab);
  currLayout->setContentsMargins(10, 20, 20, 20);

  currencyInput = new QDoubleSpinBox();
  currencyInput->setRange(0, 1e9);
  currencyInput->setDecimals(2);
  currencyInput->setValue(1);
  currencyInput->setFixedHeight(35);

  currencyFrom = new QComboBox();
  currencyFrom->addItems(currencyManager->getAvailableCurrencies());
  currencyFrom->setFixedHeight(35);

  currencyTo = new QComboBox();
  currencyTo->addItems(currencyManager->getAvailableCurrencies());
  currencyTo->setCurrentText("IDR");
  currencyTo->setFixedHeight(35);

  currencyResult = new QLabel("Result: 17000.00 IDR");
  currencyResult->setTextInteractionFlags(Qt::TextSelectableByMouse);
  currencyResult->setStyleSheet("font-size: 16px; font-weight: bold; color: #ffffff;");

  currencyStatus = new QLabel("Rates: Offline (Using Fallback 1 USD = 17000 IDR)");
  currencyStatus->setStyleSheet("color: #aaaaaa; font-size: 11px;");

  QPushButton* fetchBtn = new QPushButton("Fetch Real-time Rates");
  connect(fetchBtn, &QPushButton::clicked, this, &CalculatorWindow::onCurrencyFetchRates);

  connect(
    currencyInput, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
    &CalculatorWindow::onCurrencyConvert
  );
  connect(
    currencyFrom, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
    &CalculatorWindow::onCurrencyConvert
  );
  connect(
    currencyTo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
    &CalculatorWindow::onCurrencyConvert
  );

  currLayout->addWidget(new QLabel("Amount:"));
  currLayout->addWidget(currencyInput);
  currLayout->addSpacing(10);
  currLayout->addWidget(new QLabel("From:"));
  currLayout->addWidget(currencyFrom);
  currLayout->addSpacing(10);
  currLayout->addWidget(new QLabel("To:"));
  currLayout->addWidget(currencyTo);
  currLayout->addSpacing(20);

  QHBoxLayout* currResLayout = new QHBoxLayout();
  currResLayout->addWidget(currencyResult);
  QPushButton* copyCurrBtn = new QPushButton("Copy");
  copyCurrBtn->setFixedSize(80, 35);
  connect(copyCurrBtn, &QPushButton::clicked, this, [this]() {
    QApplication::clipboard()->setText(currencyResult->text().remove("Result: "));
  });
  currResLayout->addWidget(copyCurrBtn);
  currResLayout->addStretch();
  currLayout->addLayout(currResLayout);
  currLayout->addSpacing(15);

  currLayout->addWidget(fetchBtn);
  currLayout->addWidget(currencyStatus);
  currLayout->addStretch();
  stackedWidget->addWidget(currTab);

  // --- Programmer Tab ---
  QWidget*     progTab    = new QWidget();
  QVBoxLayout* progLayout = new QVBoxLayout(progTab);
  progLayout->setContentsMargins(10, 0, 10, 10);

  progHistory = new QLabel("");
  progHistory->setAlignment(Qt::AlignRight);
  progHistory->setStyleSheet("color: #888888; font-size: 14px;");
  progLayout->addWidget(progHistory);

  progDisplay = new QLineEdit("0");
  progDisplay->setReadOnly(true);
  progDisplay->setAlignment(Qt::AlignRight);
  progDisplay->setFocusPolicy(Qt::NoFocus);
  progDisplay->setStyleSheet(
    "background-color: transparent; border: none; font-size: 32px; "
    "font-weight: bold; margin-bottom: 5px;"
  );
  progLayout->addWidget(progDisplay);

  progBaseCombo = new QComboBox();
  progBaseCombo->addItems({"Hexadecimal", "Decimal", "Octal", "Binary"});
  progBaseCombo->setCurrentIndex(1);  // Decimal
  progBaseCombo->setFixedHeight(35);
  connect(
    progBaseCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
    &CalculatorWindow::onProgBaseChanged
  );
  progLayout->addWidget(progBaseCombo);
  progLayout->addSpacing(5);

  QGridLayout* progGrid = new QGridLayout();
  progGrid->setSpacing(8);
  QStringList progBtnLabels = {"A", "B", "C", "D", "E", "F", "Clr", QString::fromUtf8("⌫"),
                               "7", "8", "9", "/", "4", "5", "6",   "*",
                               "1", "2", "3", "-", "0", "",  "=",   "+"};

  int   pPos = 0;
  QFont progBtnFont;
  progBtnFont.setPointSize(14);
  progBtnFont.setWeight(QFont::Medium);

  for (int i = 0; i < 6; ++i) {
    for (int j = 0; j < 4; ++j) {
      if (progBtnLabels[pPos] == "") {
        pPos++;
        continue;
      }
      QPushButton* btn = new QPushButton(progBtnLabels[pPos]);
      btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      btn->setFont(progBtnFont);
      btn->setFocusPolicy(Qt::NoFocus);

      if (QString("0123456789ABCDEF").contains(progBtnLabels[pPos])) {
        btn->setObjectName("mathDigitBtn");
      }
      else if (progBtnLabels[pPos] == "=") {
        btn->setObjectName("mathEqBtn");
      }
      else {
        btn->setObjectName("mathOpBtn");
      }

      if (progBtnLabels[pPos] == "0") {
        progGrid->addWidget(btn, i, j, 1, 2);
        j++;
      }
      else {
        progGrid->addWidget(btn, i, j);
      }

      if (progBtnLabels[pPos] == "Clr") {
        connect(btn, &QPushButton::clicked, this, &CalculatorWindow::onProgClear);
      }
      else if (progBtnLabels[pPos] == QString::fromUtf8("⌫")) {
        connect(btn, &QPushButton::clicked, this, &CalculatorWindow::onProgBackspace);
      }
      else if (progBtnLabels[pPos] == "=") {
        connect(btn, &QPushButton::clicked, this, &CalculatorWindow::onProgCalculate);
      }
      else {
        connect(btn, &QPushButton::clicked, this, &CalculatorWindow::onProgButtonClicked);
        if (
          (progBtnLabels[pPos] >= "0" && progBtnLabels[pPos] <= "9")
          || (progBtnLabels[pPos] >= "A" && progBtnLabels[pPos] <= "F")
        ) {
          progButtons.append(btn);
        }
      }
      pPos++;
    }
  }
  progLayout->addLayout(progGrid);
  stackedWidget->addWidget(progTab);

  // mulai konversi
  onTempConvert();
  onCurrencyConvert();

  // Fix transparent dropdowns on Linux
  const auto combos = this->findChildren<QComboBox*>();
  for (QComboBox* cb : combos) {
    cb->setItemDelegate(new QStyledItemDelegate(cb));
    if (cb->view()) {
      cb->view()->window()->setWindowFlag(Qt::FramelessWindowHint);
      cb->view()->window()->setAttribute(Qt::WA_TranslucentBackground, false);
      cb->view()->setAutoFillBackground(true);
      if (cb->view()->viewport()) {
        cb->view()->viewport()->setAutoFillBackground(true);
      }
    }
  }
}

void CalculatorWindow::updateMathUI()
{
  mathDisplay->setText(mathEngine->getDisplayValue());
  mathHistory->setText(mathEngine->getHistoryValue());
}

void CalculatorWindow::onMathButtonClicked()
{
  QPushButton* btn = qobject_cast<QPushButton*>(sender());
  if (!btn)
    return;
  mathEngine->processInput(btn->text());
  updateMathUI();
}

void CalculatorWindow::onMathCalculate()
{
  mathEngine->calculate();
  updateMathUI();
}

void CalculatorWindow::onMathClear()
{
  mathEngine->clear();
  updateMathUI();
}

void CalculatorWindow::onMathBackspace()
{
  mathEngine->backspace();
  updateMathUI();
}

void CalculatorWindow::onTempConvert()
{
  tempResult->setText(
    ConversionEngine::convertTemperature(
      tempInput->value(), tempFrom->currentIndex(), tempTo->currentIndex()
    )
  );
}

void CalculatorWindow::onNumConvert()
{
  numResult->setText(
    ConversionEngine::convertNumber(
      numInput->text(), numFrom->currentIndex(), numTo->currentIndex()
    )
  );
}

void CalculatorWindow::onCurrencyConvert()
{
  currencyResult->setText(currencyManager->convert(
    currencyInput->value(), currencyFrom->currentText(), currencyTo->currentText()
  ));
}

void CalculatorWindow::onCurrencyFetchRates()
{
  currencyStatus->setText("Fetching rates...");
  currencyManager->fetchRates();
}

void CalculatorWindow::onCurrencyRatesUpdated()
{
  currencyStatus->setText(currencyManager->getStatusString());

  // Refresh dropdowns if new currencies arrived
  QString currFrom = currencyFrom->currentText();
  QString currTo   = currencyTo->currentText();

  currencyFrom->blockSignals(true);
  currencyTo->blockSignals(true);

  currencyFrom->clear();
  currencyTo->clear();
  currencyFrom->addItems(currencyManager->getAvailableCurrencies());
  currencyTo->addItems(currencyManager->getAvailableCurrencies());

  currencyFrom->setCurrentText(currFrom);
  currencyTo->setCurrentText(currTo);

  currencyFrom->blockSignals(false);
  currencyTo->blockSignals(false);

  onCurrencyConvert();
}

void CalculatorWindow::onCurrencyError(const QString& msg)
{
  currencyStatus->setText(currencyManager->getStatusString() + " (" + msg + ")");
}

void CalculatorWindow::onProgButtonClicked()
{
  QPushButton* btn = qobject_cast<QPushButton*>(sender());
  if (!btn)
    return;
  progEngine->processInput(btn->text());
}

void CalculatorWindow::onProgCalculate() { progEngine->calculate(); }

void CalculatorWindow::onProgClear() { progEngine->clear(); }

void CalculatorWindow::onProgBackspace() { progEngine->backspace(); }

void CalculatorWindow::onProgBaseChanged(int index) { progEngine->changeBase(index); }

void CalculatorWindow::onProgDisplayUpdated()
{
  progDisplay->setText(progEngine->getDisplayValue());
  progHistory->setText(progEngine->getHistoryValue());

  int index = progBaseCombo->currentIndex();
  for (QPushButton* btn : progButtons) {
    QString t      = btn->text();
    bool    enable = false;
    if (index == 0)
      enable = true;        // Hex
    else if (index == 1) {  // Dec
      if (t >= "0" && t <= "9")
        enable = true;
    }
    else if (index == 2) {  // Oct
      if (t >= "0" && t <= "7")
        enable = true;
    }
    else if (index == 3) {  // Bin
      if (t == "0" || t == "1")
        enable = true;
    }
    btn->setEnabled(enable);
  }
}

void CalculatorWindow::keyPressEvent(QKeyEvent* event)
{
  if (!sidebarList)
    return;
  int tabIdx = sidebarList->currentRow();
  if (tabIdx != 0 && tabIdx != 4) {
    QMainWindow::keyPressEvent(event);
    return;
  }

  QString text = event->text();
  int     key  = event->key();

  if (key == Qt::Key_Enter || key == Qt::Key_Return || key == Qt::Key_Equal) {
    if (tabIdx == 0)
      onMathCalculate();
    else
      onProgCalculate();
  }
  else if (key == Qt::Key_Escape) {
    if (tabIdx == 0)
      onMathClear();
    else
      onProgClear();
  }
  else if (key == Qt::Key_Backspace || key == Qt::Key_Delete || text == "\b" || text == "\x7F") {
    if (tabIdx == 0)
      onMathBackspace();
    else
      onProgBackspace();
  }
  else if (!text.isEmpty()) {
    if (tabIdx == 0 && QString("0123456789.+-*/").contains(text)) {
      mathEngine->processInput(text);
      updateMathUI();
    }
    else if (tabIdx == 4) {
      text       = text.toUpper();
      bool valid = false;
      int  base  = progEngine->getCurrentBase();
      if (QString("+-*/").contains(text))
        valid = true;
      else if (base == 16 && QString("0123456789ABCDEF").contains(text))
        valid = true;
      else if (base == 10 && QString("0123456789").contains(text))
        valid = true;
      else if (base == 8 && QString("01234567").contains(text))
        valid = true;
      else if (base == 2 && QString("01").contains(text))
        valid = true;

      if (valid)
        progEngine->processInput(text);
      else
        QMainWindow::keyPressEvent(event);
    }
    else {
      QMainWindow::keyPressEvent(event);
    }
  }
  else {
    QMainWindow::keyPressEvent(event);
  }
}

void CalculatorWindow::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton) {
    dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
    event->accept();
  }
}

void CalculatorWindow::mouseMoveEvent(QMouseEvent* event)
{
  if (event->buttons() & Qt::LeftButton) {
    move(event->globalPosition().toPoint() - dragPosition);
    event->accept();
  }
}
