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
#include <memory>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  std::vector<std::shared_ptr<Poppler::Document>> docs;
  for(size_t i = 1;i < argc;i++)
	  docs.emplace_back(Poppler::Document::load(argv[i]));

  QPrintPreviewDialog *pd = new QPrintPreviewDialog();
  
  pd->connect(pd, &QPrintPreviewDialog::paintRequested,
	      [&](QPrinter* printer) {
	  printer->setResolution(300);

	auto resolution = printer->resolution();
		QPainter painter;
		if (! painter.begin(printer)) { // failed to open file
		  qWarning("failed to open file, is it writable?");
		  return;
		}
		bool isFirst = true;
		for (auto& doc : docs) {
			for (size_t i = 0;i < 1;i++) {
				if (!isFirst)
					printer->newPage();
				isFirst = false; 

				std::unique_ptr<Poppler::Page> page(doc->page(i));
				auto mult = 1.0;
				auto image = page->renderToImage(resolution * mult, resolution * mult);
				if (!image.isNull()) {
					painter.drawImage(QRectF(0, 0, image.width() / mult, image.height() / mult), image,
						QRectF(0, 0, image.width(), image.height()),
						Qt::AutoColor | Qt::DiffuseAlphaDither | Qt::DiffuseDither | Qt::PreferDither
					);
				}
			}
		}
	      });
  pd->setAttribute(Qt::WA_DeleteOnClose);
  pd->show();

  return app.exec();
}
