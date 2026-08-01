#include "CurrencyManager.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QNetworkRequest>

CurrencyManager::CurrencyManager(QObject* parent) :
    QObject(parent),
    isOnline(false)
{
  // Default fallback rates
  exchangeRates["USD"] = 1.0;
  exchangeRates["IDR"] = 17000.0;
  exchangeRates["EUR"] = 0.92;
  exchangeRates["GBP"] = 0.79;
  exchangeRates["JPY"] = 150.0;

  networkManager = new QNetworkAccessManager(this);
  connect(networkManager, &QNetworkAccessManager::finished, this, &CurrencyManager::onNetworkReply);
}

void CurrencyManager::fetchRates()
{
  // Using a free API (open.er-api.com)
  QUrl            url("https://open.er-api.com/v6/latest/USD");
  QNetworkRequest request(url);
  networkManager->get(request);
}

void CurrencyManager::onNetworkReply(QNetworkReply* reply)
{
  if (reply->error() == QNetworkReply::NoError) {
    QByteArray    response = reply->readAll();
    QJsonDocument jsonDoc  = QJsonDocument::fromJson(response);
    if (!jsonDoc.isNull() && jsonDoc.isObject()) {
      QJsonObject jsonObj = jsonDoc.object();
      if (jsonObj.contains("rates")) {
        QJsonObject rates = jsonObj["rates"].toObject();
        // Merge new rates, keeping any existing ones if missing (though the API
        // provides many)
        for (const QString& key : rates.keys()) {
          exchangeRates[key] = rates[key].toDouble();
        }
        isOnline = true;
        emit ratesUpdated();
      }
    }
  }
  else {
    isOnline = false;
    emit errorOccurred(reply->errorString());
  }
  reply->deleteLater();
}

QString CurrencyManager::convert(double amount, const QString& from, const QString& to) const
{
  if (!exchangeRates.contains(from) || !exchangeRates.contains(to)) {
    return "Result: Error";
  }

  double rateFrom    = exchangeRates.value(from);
  double rateTo      = exchangeRates.value(to);
  double amountInUSD = amount / rateFrom;
  double result      = amountInUSD * rateTo;

  return QString("Result: %1 %2").arg(result, 0, 'f', 2).arg(to);
}

QStringList CurrencyManager::getAvailableCurrencies() const { return exchangeRates.keys(); }

QString CurrencyManager::getStatusString() const
{
  if (isOnline) {
    return "Rates: Real-time via API";
  }
  else {
    return "Rates: Offline (Using Fallbacks)";
  }
}
