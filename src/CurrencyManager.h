#ifndef CURRENCYMANAGER_H
#define CURRENCYMANAGER_H

#include <QMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QString>

class CurrencyManager: public QObject
{
  Q_OBJECT
public:
  explicit CurrencyManager(QObject* parent = nullptr);

  void        fetchRates();
  QString     convert(double amount, const QString& from, const QString& to) const;
  QStringList getAvailableCurrencies() const;
  QString     getStatusString() const;

signals:
  void ratesUpdated();
  void errorOccurred(const QString& errorMsg);

private slots:
  void onNetworkReply(QNetworkReply* reply);

private:
  QMap<QString, double>  exchangeRates;
  QNetworkAccessManager* networkManager;
  bool                   isOnline;
};

#endif  // CURRENCYMANAGER_H
