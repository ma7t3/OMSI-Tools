#include "wdgeditor.h"
#include "ui_wdgeditor.h"

wdgEditor::wdgEditor(QWidget *parent, OCFont *font)
    : QWidget(parent)
    , ui(new Ui::wdgEditor),
    _font(font)
{
    ui->setupUi(this);

    ui->btnNextResult->setEnabled(false);
    ui->btnFind->setEnabled(false);

    ui->gbxSearchChar->setVisible(false);

    ui->tvwChars->setModel(model);
    connect(ui->tvwChars->selectionModel(), &QItemSelectionModel::currentRowChanged, this, [this](){ ui->tvwChars->selectionModel()->blockSignals(true); switchSelection(); reloadUi(); ui->tvwChars->selectionModel()->blockSignals(false); });

    reloadUi();
    switchSelection();
}

wdgEditor::~wdgEditor()
{
    delete ui;
}

void wdgEditor::on_btnNewChar_clicked()
{
    if (!_font->selection.contains(OCFont::Selection::Font)) return;

    int prevPixelRow = ui->sbxHighestPixelInFontRow->value();

    QPersistentModelIndex currentIndex = ui->tvwChars->currentIndex();
    QPair<int, int> currentSelection(currentIndex.parent().row(), currentIndex.row());

    /* 'Adding characters' policy - if selection is...
     *  - a font: place character on bottom
     *  - a character: place character under current character
     */

    // Add new char to object
     if (!_font->selection.contains(OCFont::Selection::Character)) // font is primarily selected
        _font->fonts[_font->selection[OCFont::Selection::Font]].characters.append(OCFont::SingleFont::Character());
    else // character is primarily selected
        _font->fonts[_font->selection[OCFont::Selection::Font]].characters.insert(ui->tvwChars->currentIndex().row() + 1, OCFont::SingleFont::Character());

    emit setModified(true);
    reloadUi();
    switchSelection();

    // Control current selection
    if (!_font->selection.contains(OCFont::Selection::Character)) // font is primarily selected
        ui->tvwChars->setCurrentIndex(model->index(_font->fonts[currentSelection.second].characters.count() - 1, 0, model->index(currentSelection.second, 0)));
    else // character is primarily selected
        ui->tvwChars->setCurrentIndex(model->index(ui->tvwChars->currentIndex().row() + 1, 0, model->index(currentSelection.first, 0)));

    ui->stwProperties->setCurrentIndex(1);
    ui->ledCharacter->setFocus();

    if (set.read(objectName(), "keepPixelRow").toBool()) ui->sbxHighestPixelInFontRow->setValue(prevPixelRow);
}

void wdgEditor::on_btnNewFont_clicked()
{
    QPersistentModelIndex currentIndex = ui->tvwChars->currentIndex();
    QPair<int, int> currentSelection(currentIndex.parent().row(), currentIndex.row());

    /* 'Adding font' policy: place font under current font; select it
     */

    // Add new font to object
    if (_font->fonts.isEmpty()) // font file is empty
        _font->fonts.append(OCFont::SingleFont());
    else if (!_font->selection.contains(OCFont::Selection::Character)) // font is primarily selected
        _font->fonts.insert(currentSelection.second + 1, OCFont::SingleFont());
    else // character is primarily selected
        _font->fonts.insert(currentSelection.first + 1, OCFont::SingleFont());

    emit setModified(true);
    reloadUi();
    switchSelection();

    // Control current selection
    if (!_font->selection.contains(OCFont::Selection::Character)) // font is primarily selected
        ui->tvwChars->setCurrentIndex(model->index(currentSelection.second + 1, 0));
    else // character is primarily selected
        ui->tvwChars->setCurrentIndex(model->index(currentSelection.first + 1, 0));

    ui->stwProperties->setCurrentIndex(0);
    ui->ledFontName->setFocus();
}

void wdgEditor::on_btnDeleteSelection_clicked()
{
    if (_font->selection.contains(OCFont::Selection::Character))
    {
        _font->fonts[_font->selection[OCFont::Selection::Font]].characters.removeAt(ui->tvwChars->currentIndex().row());

        if (_font->fonts[_font->selection[OCFont::Selection::Font]].characters.isEmpty())
            ui->tvwChars->setCurrentIndex(ui->tvwChars->currentIndex().parent());
    }
    else if (_font->selection.contains(OCFont::Selection::Font)) _font->fonts.removeAt(ui->tvwChars->currentIndex().row());
    else return;

    reloadUi();
    switchSelection();

    emit setModified(true);
}

void wdgEditor::on_btnMoveUp_clicked()
{
    if (_font->fonts[_font->selection[OCFont::Selection::Font]].characters.count() != 0 && ui->tvwChars->currentIndex().parent().row() != 1)
        moveChar(ui->tvwChars->currentIndex().row(), Move::Up);
}

void wdgEditor::on_btnMoveDown_clicked()
{
    if (ui->tvwChars->currentIndex().row() != (_font->fonts[_font->selection[OCFont::Selection::Font]].characters.count() - 1) && ui->tvwChars->currentIndex().parent().row() != 1)
        moveChar(ui->tvwChars->currentIndex().row(), Move::Down);
}

void wdgEditor::on_ledCharacter_textChanged(const QString &arg1)
{
    if (!_font->selection.contains(OCFont::Selection::Character)) return;
    if (_font->fonts[_font->selection[OCFont::Selection::Font]].characters.count() != 0 && ui->tvwChars->currentIndex().row() != -1)
    {
        _font->fonts[_font->selection[OCFont::Selection::Font]].characters[ui->tvwChars->currentIndex().row()].setCharacter(arg1);
        reloadUi();
    }

    checkCharValidity();
    emit setModified(true);
}

void wdgEditor::on_sbxLeftPixel_valueChanged(int arg1)
{
    if (!_font->selection.contains(OCFont::Selection::Character)) return;
    if (_font->fonts[_font->selection[OCFont::Selection::Font]].characters.count() != 0 && ui->tvwChars->currentIndex().row() != -1)
    {
        _font->fonts[_font->selection[OCFont::Selection::Font]].characters[ui->tvwChars->currentIndex().row()].setLeftPixel(arg1);
        reloadUi();
    }

    checkCharValidity();
    emit setModified(true);
}

void wdgEditor::on_sbxRightPixel_valueChanged(int arg1)
{
    if (!_font->selection.contains(OCFont::Selection::Character)) return;
    if (_font->fonts[_font->selection[OCFont::Selection::Font]].characters.count() != 0 && ui->tvwChars->currentIndex().row() != -1)
    {
        _font->fonts[_font->selection[OCFont::Selection::Font]].characters[ui->tvwChars->currentIndex().row()].setRightPixel(arg1);
        reloadUi();
    }

    checkCharValidity();
    emit setModified(true);
}

void wdgEditor::on_sbxHighestPixelInFontRow_valueChanged(int arg1)
{
    if (!_font->selection.contains(OCFont::Selection::Character)) return;
    if (_font->fonts[_font->selection[OCFont::Selection::Font]].characters.count() != 0 && ui->tvwChars->currentIndex().row() != -1)
    {
        _font->fonts[_font->selection[OCFont::Selection::Font]].characters[ui->tvwChars->currentIndex().row()].setHighestPixelInFontRow(arg1);
        reloadUi();
    }

    checkCharValidity();
    emit setModified(true);
}

void wdgEditor::on_btnFind_clicked() // TODO
{
    if (!_font->selection.contains(OCFont::Selection::Character)) return;
    currentSearch = ui->ledSearch->text();

    qDebug().noquote() << "Find char: '" + currentSearch + "'";

    ui->btnNextResult->setEnabled(true);

    // Search for char
    ui->tvwChars->setCurrentIndex(model->index(0, 0));

    for (int i = 0; i < _font->fonts[_font->selection[OCFont::Selection::Font]].characters.count(); i++)
    {
        if (_font->fonts[_font->selection[OCFont::Selection::Font]].characters.at(i).character() == currentSearch)
        {
            switchSelection();
            return;
        }

        ui->tvwChars->setCurrentIndex(model->index(i + 1, 0));
    }

    QMessageBox::information(this, tr("Character not found"), tr("The entered character could not be found."));
    qDebug() << "Character not found.";
    currentSearch = "";
    ui->btnNextResult->setEnabled(false);
}

void wdgEditor::on_btnNextResult_clicked()
{
    if (!_font->selection.contains(OCFont::Selection::Character)) return;
    qDebug() << "Go to next search result";
    if (currentSearch != "")
    {
        if (_font->fonts[_font->selection[OCFont::Selection::Font]].characters.count() == 1)
            return;

        int i = ui->tvwChars->currentIndex().row();
        
        QList<OCFont::SingleFont::Character> tempList;
        tempList = _font->fonts[_font->selection[OCFont::Selection::Font]].characters;
        tempList.removeAt(ui->tvwChars->currentIndex().row());

        bool secondRound = false;

        if (i >= tempList.count())
        {
            i = 0;
            qDebug() << "Search: End of font, go to top";
            //ui->statusbar->showMessage(tr("The end of the font was reached, search from top"), 4000);
            secondRound = true;
        }

        forever
        {
            if (i > tempList.count() - 1)
            {
                qDebug() << "Search: Not other char found";
                //ui->statusbar->showMessage(tr("No other character found according to the search criteria."), 4000);
                switchSelection();
                return;
            }
            else if (tempList.at(i).character() == currentSearch)
            {
                if (i > ui->tvwChars->currentIndex().row() || i == ui->tvwChars->currentIndex().row())
                    i++;
                ui->tvwChars->setCurrentIndex(model->index(i, 0));
                goto end;
            }

            if (!secondRound && (i == tempList.count() - 1))
            {
                i = -1;
                secondRound = true;
            }
            else if (secondRound && (i == ui->tvwChars->currentIndex().row() - 1))
            {
                qDebug() << "Search: Not other char found";
                //ui->statusbar->showMessage(tr("No other character found according to the search criteria."), 4000);
                switchSelection();
                return;
            }

            i++;
        }
    end:
        if (secondRound)
        {
            qDebug() << "Search: End of font, go to top";
            //ui->statusbar->showMessage(tr("The end of the font was reached, search from top"), 4000);
        }

        switchSelection();
    }
}

void wdgEditor::on_btnEditorPreferences_clicked()
{
    WPREFERENCES->setWindowModality(Qt::ApplicationModal);
    WPREFERENCES->show();
}

// Reloads prop editor state
void wdgEditor::switchSelection()
{
    _font->selection.clear();

    QModelIndex currentIndex = ui->tvwChars->currentIndex();

    if (currentIndex.parent().row() == -1)
    {
        if (currentIndex.row() == -1) // selection is nothing
        {
            ui->stwProperties->setEnabled(false);
            ui->btnNewChar->setEnabled(false);
        }
        else // selection is a font
        {
            _font->selection[OCFont::Selection::Font] = currentIndex.row();

            ui->stwProperties->setCurrentIndex(0);
            ui->stwProperties->setEnabled(true);
            ui->btnNewChar->setEnabled(true);

            ui->ledFontName->setText(_font->fonts[currentIndex.row()].name());
            ui->ledColorTexture->setText(_font->fonts[currentIndex.row()].colorTexture());
            ui->ledAlphaTexture->setText(_font->fonts[currentIndex.row()].alphaTexture());
            ui->sbxMaxHeigthOfChars->setValue(_font->fonts[currentIndex.row()].maxHeightOfChars());
            ui->sbxDistanceBetweenChars->setValue(_font->fonts[currentIndex.row()].distanceBetweenChars());
        }
    }
    else // selection is a character
    {
        _font->selection[OCFont::Selection::Font] = currentIndex.parent().row();
        _font->selection[OCFont::Selection::Character] = currentIndex.row();

        ui->stwProperties->setCurrentIndex(1);
        ui->stwProperties->setEnabled(true);
        ui->btnNewChar->setEnabled(true);

        OCFont::SingleFont::Character character = _font->fonts[currentIndex.parent().row()].characters.at(currentIndex.row());

        ui->ledCharacter->setText(character.character());
        ui->sbxLeftPixel->setValue(character.leftPixel());
        ui->sbxRightPixel->setValue(character.rightPixel());
        ui->sbxHighestPixelInFontRow->setValue(character.highestPixelInFontRow());
    }

    checkCharValidity();
    emit reloadPreview();
}

void wdgEditor::checkCharValidity()
{
    if (!_font->selection.contains(OCFont::Selection::Character)) return; // TODO: Check of all characters (with in-TreeView stylesheeting)

    ui->lblCharacter->setStyleSheet("");
    ui->lblRightPixel->setStyleSheet("");
    ui->lblLeftPixel->setStyleSheet("");
    ui->lblHighestPixelInFontRow->setStyleSheet("");

    if (_font->fonts[_font->selection[OCFont::Selection::Font]].characters.isEmpty() || (ui->tvwChars->currentIndex().row() == -1)) return;

    OCFont::SingleFont::Character character = _font->fonts[_font->selection[OCFont::Selection::Font]].characters.at(ui->tvwChars->currentIndex().row());

    if (character.character().isEmpty()) ui->lblCharacter->setStyleSheet("color:red");
    else
    {
        QStringList tempCharList;
        foreach (OCFont::SingleFont::Character current, _font->fonts[_font->selection[OCFont::Selection::Font]].characters)
            tempCharList << current.character();

        if (tempCharList.indexOf(character.character()) != -1)
        {
            tempCharList.removeAt(tempCharList.indexOf(character.character()));
            if (tempCharList.indexOf(character.character()) != -1)
                ui->lblCharacter->setStyleSheet("color:red");
        }
    }

    if (character.leftPixel() > character.rightPixel())
    {
        ui->lblRightPixel->setStyleSheet("color:goldenrod");
        ui->lblLeftPixel->setStyleSheet("color:goldenrod");
    }

    if (character.rightPixel() == -1) ui->lblRightPixel->setStyleSheet("color:red");
    if (character.leftPixel() == -1) ui->lblLeftPixel->setStyleSheet("color:red");
    if (character.highestPixelInFontRow() == -1) ui->lblHighestPixelInFontRow->setStyleSheet("color:red");

    if (QFile(set.read("main", "mainDir").toString() + "/Fonts/" + _font->fonts[_font->selection[OCFont::Selection::Font]].alphaTexture()).exists())
    {
        QImage alphaTexture(set.read("main", "mainDir").toString() + "/Fonts/" + _font->fonts[_font->selection[OCFont::Selection::Font]].alphaTexture());

        if (alphaTexture.width() != 0 || alphaTexture.height() != 0)
        {
            if (character.rightPixel() > QString::number(alphaTexture.width()).toInt())
                ui->lblRightPixel->setStyleSheet("color:red");

            if (character.leftPixel() > QString::number(alphaTexture.width()).toInt())
                ui->lblLeftPixel->setStyleSheet("color:red");

            if (character.highestPixelInFontRow() > QString::number(alphaTexture.width()).toInt())
                ui->lblHighestPixelInFontRow->setStyleSheet("color:red");
        }
    }
}

void wdgEditor::checkPropValidity()
{
    if (!_font->selection.contains(OCFont::Selection::Font)) return;

    ui->lblFontName->setStyleSheet("");
    ui->lblColorTexture->setStyleSheet("");
    ui->lblAlphaTexture->setStyleSheet("");
    ui->lblMaxHeigthOfChars->setStyleSheet("");
    ui->lblDistanceBetweenChars->setStyleSheet("");

    if (_font->fonts[_font->selection[OCFont::Selection::Font]].name().isEmpty())
        ui->lblFontName->setStyleSheet("color:red");

    if (!_font->fonts[_font->selection[OCFont::Selection::Font]].colorTexture().isEmpty() && !QFile(set.read("main", "mainDir").toString() + "/Fonts/" + _font->fonts[_font->selection[OCFont::Selection::Font]].colorTexture()).exists())
        ui->lblColorTexture->setStyleSheet("color:red");

    if (_font->fonts[_font->selection[OCFont::Selection::Font]].alphaTexture().isEmpty() || !QFile(set.read("main", "mainDir").toString() + "/Fonts/" + _font->fonts[_font->selection[OCFont::Selection::Font]].alphaTexture()).exists())
        ui->lblAlphaTexture->setStyleSheet("color:red");

    if (ui->sbxMaxHeigthOfChars->text().isEmpty() || (_font->fonts[_font->selection[OCFont::Selection::Font]].maxHeightOfChars() == 0))
        ui->lblMaxHeigthOfChars->setStyleSheet("color:red");

    if (ui->sbxDistanceBetweenChars->text().isEmpty())
        ui->lblDistanceBetweenChars->setStyleSheet("color:red");

    if (QFile(set.read("main", "mainDir").toString() + "/Fonts/" + _font->fonts[_font->selection[OCFont::Selection::Font]].alphaTexture()).exists())
    {
        QImage alphaTexture(set.read("main", "mainDir").toString() + "/Fonts/" + _font->fonts[_font->selection[OCFont::Selection::Font]].alphaTexture());

        if (alphaTexture.width() != 0 || alphaTexture.height() != 0)
        {
            if (_font->fonts[_font->selection[OCFont::Selection::Font]].maxHeightOfChars() > QString::number(alphaTexture.height()).toInt())
                ui->lblMaxHeigthOfChars->setStyleSheet("color:red");

            if (_font->fonts[_font->selection[OCFont::Selection::Font]].distanceBetweenChars() > QString::number(alphaTexture.width()).toInt())
                ui->lblDistanceBetweenChars->setStyleSheet("color:red");
        }
    }
}

void wdgEditor::moveChar(int sel, Move action)
{
    if (!_font->selection.contains(OCFont::Selection::Font)) return;

    if (action == Move::Up && sel == 0) return;
    if (action == Move::Down && _font->selection.contains(OCFont::Selection::Character) && sel > (_font->fonts[_font->selection[OCFont::Selection::Font]].characters.count() - 1)) return;
    if (action == Move::Down && _font->selection.contains(OCFont::Selection::Character) && sel > (_font->fonts.count() - 1)) return;

    int moving = action == Move::Up ? sel - 1 : sel + 1;

    if (_font->selection.contains(OCFont::Selection::Character))
        _font->fonts[_font->selection[OCFont::Selection::Font]].characters.move(sel, moving);
    else if (_font->selection.contains(OCFont::Selection::Font))
        _font->fonts.move(sel, moving);
    else
        return;

    emit setModified(true);
    reloadUi();

    if (_font->selection.contains(OCFont::Selection::Character)) ui->tvwChars->setCurrentIndex(model->index(moving, 0, ui->tvwChars->currentIndex().parent()));
    else if (_font->selection.contains(OCFont::Selection::Font)) ui->tvwChars->setCurrentIndex(model->index(moving, 0));
}

// Reloads TreeView
void wdgEditor::reloadUi()
{
    int currentScrollbarPosition = ui->tvwChars->verticalScrollBar()->value();
    QPersistentModelIndex currentIndex = ui->tvwChars->currentIndex();
    QPair<int, int> currentSelection(currentIndex.parent().row(), currentIndex.row());

    QList<bool> expansions; // TODO: clear on font reload

    for (int i = 0; i < model->rowCount(); i++)
        expansions << ui->tvwChars->isExpanded(model->index(i, 0)); // TODO: implement for delete, move!

    model->clear();

    for (int i = 0; i < _font->fonts.count(); i++)
    {
        OCFont::SingleFont font = _font->fonts[i];

        QStandardItem *fontItem = new QStandardItem(font.name().isEmpty() ? QString("(%1)").arg(tr("unnamed")) : font.name());
        fontItem->setRowCount(font.characters.count());

        for (int j = 0; j < font.characters.count(); j++) fontItem->setChild(j, 0, new QStandardItem(font.characters[j].character().isEmpty() ? QString("(%1)").arg(tr("unnamed")) : font.characters[j].character()));

        model->invisibleRootItem()->appendRow(fontItem);
    }

    // Re-set expansions
    for (int i = 0; i < expansions.count(); i++) ui->tvwChars->setExpanded(model->index(i, 0), expansions[i]);

    QModelIndex newSelection;
    QModelIndex prevNewSelection;

    if (currentSelection.first == -1)
    {
        newSelection = model->index(currentSelection.second, 0);
        prevNewSelection = model->index(currentSelection.second - 1, 0);
    }
    else
    {
        newSelection = model->index(currentSelection.second, 0, model->index(currentSelection.first, 0));
        prevNewSelection = model->index(currentSelection.second - 1, 0, model->index(currentSelection.first, 0));
    }

    if (newSelection.isValid())
        ui->tvwChars->setCurrentIndex(newSelection);
    else if (prevNewSelection.isValid())
        ui->tvwChars->setCurrentIndex(prevNewSelection);

    ui->tvwChars->verticalScrollBar()->setValue(currentScrollbarPosition);

    ui->lblStatistics->setText(tr("%1 fonts, %2 characters total").arg(QString::number(_font->fonts.count()), QString::number(_font->totalCharacterCount())));

    // checkCharValidity();
    // checkPropValidity();

    ui->btnDeleteSelection->setDisabled(model->rowCount() == 0);

    ui->btnMoveUp->setEnabled(ui->tvwChars->currentIndex().row() > 0);
    if (_font->selection.contains(OCFont::Selection::Character))
        ui->btnMoveDown->setEnabled(ui->tvwChars->currentIndex().row() < (_font->fonts[ui->tvwChars->currentIndex().parent().row()].characters.count() - 1));
    else if (_font->selection.contains(OCFont::Selection::Font))
        ui->btnMoveDown->setEnabled(ui->tvwChars->currentIndex().row() < (_font->fonts.count() - 1));
    else
        ui->btnMoveDown->setEnabled(false);

    // TODO FOR Char position preview: Reload tex preview here (maybe in thread)
}

void wdgEditor::on_btnTest_clicked()
{
    reloadUi();
}

void wdgEditor::on_ledFontName_textChanged(const QString &arg1)
{
    if (!_font->selection.contains(OCFont::Selection::Font)) return;
    if (_font->selection[OCFont::Selection::Font] == -1) return;

    _font->fonts[_font->selection[OCFont::Selection::Font]].setName(arg1);

    reloadUi();
    checkPropValidity();
    checkCharValidity();
    emit setModified(true);
}

void wdgEditor::on_ledColorTexture_textChanged(const QString &arg1)
{
    if (!_font->selection.contains(OCFont::Selection::Font)) return;
    if (_font->selection[OCFont::Selection::Font] == -1) return;

    _font->fonts[_font->selection[OCFont::Selection::Font]].setColorTexture(arg1);

    reloadUi();
    checkPropValidity();
    checkCharValidity();
    emit setModified(true);

    emit reloadPreview();
}

void wdgEditor::on_btnColorTexture_clicked()
{
    if (!_font->selection.contains(OCFont::Selection::Font)) return;
    if (_font->selection[OCFont::Selection::Font] == -1) return;

    QString filename = QFileDialog::getOpenFileName(this, tr("Select color texture..."), set.read("main", "mainDir").toString() + "/Fonts", tr("Bitmap picture") + " (*.bmp)");

    if (filename.isEmpty()) return;

    _font->fonts[_font->selection[OCFont::Selection::Font]].setColorTexture(filename.remove(0, QString(set.read("main", "mainDir").toString() + "/Fonts").size() + 1));

    if (ui->ledColorTexture->text() != _font->fonts[_font->selection[OCFont::Selection::Font]].colorTexture())
    {
        qDebug() << QString("New color texture: '%1'").arg(filename);
        emit setModified(true);
    }

    ui->ledColorTexture->setText(_font->fonts[_font->selection[OCFont::Selection::Font]].colorTexture());
}

void wdgEditor::on_ledAlphaTexture_textChanged(const QString &arg1)
{
    if (!_font->selection.contains(OCFont::Selection::Font)) return;
    if (_font->selection[OCFont::Selection::Font] == -1) return;

    _font->fonts[_font->selection[OCFont::Selection::Font]].setAlphaTexture(arg1);

    reloadUi();
    checkPropValidity();
    checkCharValidity();
    emit setModified(true);

    emit reloadPreview();
}

void wdgEditor::on_btnAlphaTexture_clicked()
{
    if (!_font->selection.contains(OCFont::Selection::Font)) return;
    if (_font->selection[OCFont::Selection::Font] == -1) return;

    QString filename = QFileDialog::getOpenFileName(this, tr("Select alpha texture..."), set.read("main", "mainDir").toString() + "/Fonts", tr("Bitmap picture") + " (*.bmp)");
    if (filename.isEmpty()) return;

    _font->fonts[_font->selection[OCFont::Selection::Font]].setAlphaTexture(filename.remove(0, QString(set.read("main", "mainDir").toString() + "/Fonts").size() + 1));

    if (ui->ledAlphaTexture->text() != _font->fonts[_font->selection[OCFont::Selection::Font]].alphaTexture())
    {
        qDebug() << QString("New alpha texture: '%1'").arg(filename);
        emit setModified(true);
    }

    ui->ledAlphaTexture->setText(_font->fonts[_font->selection[OCFont::Selection::Font]].alphaTexture());
}

void wdgEditor::on_sbxMaxHeigthOfChars_valueChanged(int arg1)
{
    if (!_font->selection.contains(OCFont::Selection::Font)) return;
    if (_font->selection[OCFont::Selection::Font] == -1) return;

    _font->fonts[_font->selection[OCFont::Selection::Font]].setMaxHeightOfChars(arg1);

    reloadUi();
    checkPropValidity();
    checkCharValidity();
    emit setModified(true);
}

void wdgEditor::on_sbxDistanceBetweenChars_valueChanged(int arg1)
{
    if (!_font->selection.contains(OCFont::Selection::Font)) return;
    if (_font->selection[OCFont::Selection::Font] == -1) return;

    _font->fonts[_font->selection[OCFont::Selection::Font]].setDistanceBetweenChars(arg1);

    reloadUi();
    checkPropValidity();
    checkCharValidity();
    emit setModified(true);
}
