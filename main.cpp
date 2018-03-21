#include "poppler/qt5/poppler-qt5.h"
#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QPrintPreviewDialog>
#include <QPrinterInfo>
#include <QShowEvent>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtGui/QDesktopServices>
#include <QtPrintSupport/QPrintDialog>
#include <QtPrintSupport/QPrinter>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  bool hasPrinters = !QPrinterInfo::availablePrinters().empty();

  bool showPreview = argc > 2 && std::string(argv[1]) == "--preview";
  size_t startPdf = showPreview ? 2 : 1;

  std::vector<std::shared_ptr<Poppler::Document>> docs;
  for (size_t i = startPdf; i < argc; i++)
    docs.emplace_back(Poppler::Document::load(argv[i]));

  auto printFunction = [&](QPrinter *printer) {
    printer->setResolution(300);
    printer->setFullPage(true);
    auto resolution = printer->resolution();
    QPainter painter;
    if (!painter.begin(printer)) { // failed to open file
      qWarning("failed to open file, is it writable?");
      return;
    }
    bool isFirst = true;
    for (auto &doc : docs) {
      for (size_t i = 0; i < doc->numPages(); i++) {
        if (!isFirst)
          printer->newPage();
        isFirst = false;

        std::unique_ptr<Poppler::Page> page(doc->page(i));
        auto image = page->renderToImage(resolution, resolution);
        if (!image.isNull()) {
          painter.drawImage(
              QRectF(0, 0, painter.device()->width(), painter.device()->height()), image,
              QRectF(0, 0, image.width(), image.height()),
              Qt::AutoColor | Qt::DiffuseAlphaDither | Qt::DiffuseDither |
                  Qt::PreferDither);
        }
      }
    }
  };

  if (!showPreview) {
    QPrinter printer(QPrinter::HighResolution);

    if (hasPrinters) {
      QPrintDialog *pd = new QPrintDialog(&printer);

      if (pd->exec() == QDialog::Accepted) {
        printFunction(&printer);
      }
    } else {
      QString fileName = QFileDialog::getSaveFileName(
          (QWidget *)0, "Export PDF", QString(), "*.pdf");
      if (QFileInfo(fileName).suffix().isEmpty()) {
        fileName.append(".pdf");
      }

      printer.setOutputFormat(QPrinter::PdfFormat);
      printer.setPaperSize(QPrinter::A4);
      printer.setOutputFileName(fileName);
      printFunction(&printer);
    }

  } else {
    QPrintPreviewDialog *pd = new QPrintPreviewDialog();

    pd->connect(pd, &QPrintPreviewDialog::paintRequested, printFunction);
    pd->setAttribute(Qt::WA_DeleteOnClose);
    pd->show();

    return app.exec();
  }

  return 0;
}
