#include <QApplication>
#include <QtCore/QUrl>
#include <QtWebEngineWidgets/QWebEngineView>
#include <QDir>
#include <mutex>
#include <condition_variable>
#include <QtGui/QDesktopServices>
#include <QShowEvent>
#include <QtCore/QTimer>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>
#include "poppler/qt5/poppler-qt5.h"
#include <QPrintPreviewDialog>
#include <iostream>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  auto fileName = argv[1];

  auto doc = Poppler::Document::load(fileName);
  auto page = doc->page(0);

  QPrintPreviewDialog *pd = new QPrintPreviewDialog();
  
  pd->connect(pd, &QPrintPreviewDialog::paintRequested,
	      [&](QPrinter* printer) {
		auto resolution = printer->resolution();
		QPainter painter;
		if (! painter.begin(printer)) { // failed to open file
		  qWarning("failed to open file, is it writable?");
		  return;
		}

		for(size_t i = 0;i < 1;i++) {
		  if(i != 0)
		    printer->newPage();
		  std::cerr << doc->page(i)->pageSize().width() << ", " << doc->page(i)->pageSize().height() << std::endl;
		  auto mult = 1.0;
		  auto image = doc->page(i)->renderToImage(resolution * mult, resolution * mult);
		  if(!image.isNull()) {
		    painter.drawImage(QRectF(0, 0, image.width() / mult, image.height() / mult), image,
				      QRectF(0, 0, image.width(), image.height()),
				      Qt::AutoColor | Qt::DiffuseAlphaDither | Qt::DiffuseDither | Qt::PreferDither
				      );
		  }
		}
	      });
  pd->setAttribute(Qt::WA_DeleteOnClose);
  pd->show();

  return app.exec();
}
