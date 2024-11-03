#include "wdgeditor.h"
#include "ui_wdgeditor.h"

wdgEditor::wdgEditor(QWidget *parent, FontCollection *font)
    : QWidget(parent)
    , ui(new Ui::wdgEditor),
    _font(font)
{
    ui->setupUi(this);

    ui->tvwChars->setModel(model);
    connect(ui->tvwChars->selectionModel(), &QItemSelectionModel::currentRowChanged, this, [this](){ ui->tvwChars->selectionModel()->blockSignals(true); blockSignals(true); switchSelection(); reloadUi(); ui->tvwChars->selectionModel()->blockSignals(false); blockSignals(false); });

    // Create and connect actions --------------------------------------------------------------------
    actionAddFont = new QAction(QIcon::fromTheme("list-add"), "Add font"); actionAddFont->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_N));
    actionsEdit << actionAddFont;
    actionAddCharacter = new QAction(QIcon::fromTheme("list-add"), "Add character"); actionAddCharacter->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Tab));
    actionsEdit << actionAddCharacter;
    actionDeleteItem = new QAction(QIcon::fromTheme("edit-clear"), "Delete item"); actionDeleteItem->setShortcut(Qt::Key_Delete);
    actionsEdit << actionDeleteItem;
    actionMoveUp = new QAction(QIcon::fromTheme("go-up"), "Move item up"); actionMoveUp->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Up));
    actionsEdit << actionMoveUp;
    actionMoveDown = new QAction(QIcon::fromTheme("go-down"), "Move item down"); actionMoveDown->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Down));
    actionsEdit << actionMoveDown;
    actionSearch = new QAction(QIcon::fromTheme("edit-find"), "Search"); actionSearch->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F));
    actionsEdit << actionSearch; ui->ledSearch->addAction(actionSearch, QLineEdit::TrailingPosition);
    actionGoToNextError = new QAction(QIcon::fromTheme("tools-check-spelling"), "Go no next error"); actionGoToNextError->setShortcut(Qt::Key_F5);
    actionsEdit << actionGoToNextError;

    ui->tvwChars->addActions(actionsEdit);

    connect(actionAddFont, &QAction::triggered, this, &wdgEditor::addFont);
    connect(actionAddCharacter, &QAction::triggered, this, &wdgEditor::addCharacter);
    connect(actionDeleteItem, &QAction::triggered, this, &wdgEditor::deleteItem);
    connect(actionMoveUp, &QAction::triggered, this, [this]() {
        if (_font->fonts().at(_font->selection().value(FontCollection::FontSelection))->characters().count() != 0 && ui->tvwChars->currentIndex().parent().row() != 1)
            moveChar(ui->tvwChars->currentIndex().row(), Move::Up);
    });
    connect(actionMoveDown, &QAction::triggered, this, [this]() {
        if (ui->tvwChars->currentIndex().row() != (_font->fonts().at(_font->selection().value(FontCollection::FontSelection))->characters().count() - 1) && ui->tvwChars->currentIndex().parent().row() != 1)
            moveChar(ui->tvwChars->currentIndex().row(), Move::Down);
    });
    connect(actionSearch, &QAction::triggered, this, &wdgEditor::search);
    connect(actionGoToNextError, &QAction::triggered, this, &wdgEditor::goToNextError);

    connect(ui->btnAddFont, &QPushButton::clicked, this, &wdgEditor::addFont);
    connect(ui->btnAddCharacter, &QPushButton::clicked, this, &wdgEditor::addCharacter);
    connect(ui->btnDeleteItem, &QPushButton::clicked, this, &wdgEditor::deleteItem);
    connect(ui->btnMoveUp, &QPushButton::clicked, this, [this]() {
        if (_font->fonts().at(_font->selection().value(FontCollection::FontSelection))->characters().count() != 0 && ui->tvwChars->currentIndex().parent().row() != 1)
            moveChar(ui->tvwChars->currentIndex().row(), Move::Up);
    });
    connect(ui->btnMoveDown, &QPushButton::clicked, this, [this]() {
        if (ui->tvwChars->currentIndex().row() != (_font->fonts().at(_font->selection().value(FontCollection::FontSelection))->characters().count() - 1) && ui->tvwChars->currentIndex().parent().row() != 1)
            moveChar(ui->tvwChars->currentIndex().row(), Move::Down);
    });

    connect(actionAddFont, &QAction::enabledChanged, ui->btnAddFont, &QPushButton::setEnabled);
    connect(actionAddCharacter, &QAction::enabledChanged, ui->btnAddCharacter, &QPushButton::setEnabled);
    connect(actionDeleteItem, &QAction::enabledChanged, ui->btnDeleteItem, &QPushButton::setEnabled);
    connect(actionMoveUp, &QAction::enabledChanged, ui->btnMoveUp, &QPushButton::setEnabled);
    connect(actionMoveDown, &QAction::enabledChanged, ui->btnMoveDown, &QPushButton::setEnabled);

    // --------------------------------------------------------------

    ui->fraSearch->setVisible(false);

    reloadUi();
    switchSelection();
}

wdgEditor::~wdgEditor()
{
    delete ui;
}

void wdgEditor::unexpandAll()
{
    ui->tvwChars->collapseAll();
}

void wdgEditor::on_ledCharacter_textChanged(const QString &arg1)
{
    if (!_font->selection().contains(FontCollection::CharacterSelection)) return;
    if (_font->fonts().at(_font->selection().value(FontCollection::FontSelection))->characters().count() != 0 && ui->tvwChars->currentIndex().row() != -1)
    {
        _font->fonts().at(_font->selection().value(FontCollection::FontSelection))->characters().at(ui->tvwChars->currentIndex().row())->setCharacter(arg1); // TODO: need update functions??
        reloadUi();
    }

    checkCharValidity();
    emit setModified(true);
}

void wdgEditor::on_sbxLeftPixel_valueChanged(int arg1)
{
    if (!_font->selection().contains(FontCollection::CharacterSelection)) return;
    if (_font->fonts().at(_font->selection().value(FontCollection::FontSelection))->characters().count() != 0 && ui->tvwChars->currentIndex().row() != -1)
    {
        _font->fonts().at(_font->selection().value(FontCollection::FontSelection))->characters().at(ui->tvwChars->currentIndex().row())->setLeftPixel(arg1);
        reloadUi();
    }

    checkCharValidity();
    emit setModified(true);
}

void wdgEditor::on_sbxRightPixel_valueChanged(int arg1)
{
    if (!_font->selection().contains(FontCollection::CharacterSelection)) return;
    if (_font->fonts().at(_font->selection().value(FontCollection::FontSelection))->characters().count() != 0 && ui->tvwChars->currentIndex().row() != -1)
    {
        _font->fonts().at(_font->selection().value(FontCollection::FontSelection))->characters().at(ui->tvwChars->currentIndex().row())->setRightPixel(arg1);
        reloadUi();
    }

    checkCharValidity();
    emit setModified(true);
}

void wdgEditor::on_sbxHighestPixelInFontRow_valueChanged(int arg1)
{
    if (!_font->selection().contains(FontCollection::CharacterSelection)) return;
    if (_font->fonts().at(_font->selection().value(FontCollection::FontSelection))->characters().count() != 0 && ui->tvwChars->currentIndex().row() != -1)
    {
        _font->fonts().at(_font->selection().value(FontCollection::FontSelection))->characters().at(ui->tvwChars->currentIndex().row())->setHighestPixelInFontRow(arg1);
        reloadUi();
    }

    checkCharValidity();
    emit setModified(true);
}

void wdgEditor::on_btnEditorPreferences_clicked()
{
    WPREFERENCES->setWindowModality(Qt::ApplicationModal);
    WPREFERENCES->show();
}

// Reloads prop editor state
void wdgEditor::switchSelection()
{
    _font->setSelection(QMap<FontCollection::Selection, int>());

    QModelIndex currentIndex = ui->tvwChars->currentIndex();

    if (currentIndex.parent().row() == -1)
    {
        if (currentIndex.row() == -1) // selection is nothing
        {
            ui->stwProperties->setEnabled(false);
            actionAddCharacter->setEnabled(false);
        }
        else // selection is a font
        {
            _font->appendSelection(FontCollection::FontSelection, currentIndex.row());

            ui->stwProperties->setCurrentIndex(0);
            ui->stwProperties->setEnabled(true);
            actionAddCharacter->setEnabled(true);

            ui->ledFontName->setText(_font->fonts().at(currentIndex.row())->name());
            ui->ledColorTexture->setText(_font->fonts().at(currentIndex.row())->colorTexture());
            ui->ledAlphaTexture->setText(_font->fonts().at(currentIndex.row())->alphaTexture());
            ui->sbxMaxHeigthOfChars->setValue(_font->fonts().at(currentIndex.row())->maxHeightOfChars());
            ui->sbxDistanceBetweenChars->setValue(_font->fonts().at(currentIndex.row())->distanceBetweenChars());
        }
    }
    else // selection is a character
    {
        _font->appendSelection(FontCollection::FontSelection, currentIndex.parent().row());
        _font->appendSelection(FontCollection::CharacterSelection, currentIndex.row());

        ui->stwProperties->setCurrentIndex(1);
        ui->stwProperties->setEnabled(true);
        actionAddCharacter->setEnabled(true);

        Character *character = _font->fonts().at(currentIndex.parent().row())->characters().at(currentIndex.row());

        ui->ledCharacter->setText(character->character());
        ui->sbxLeftPixel->setValue(character->leftPixel());
        ui->sbxRightPixel->setValue(character->rightPixel());
        ui->sbxHighestPixelInFontRow->setValue(character->highestPixelInFontRow());
    }

    checkCharValidity();
    emit reloadPreview();
}

void wdgEditor::addFont()
{
    QPersistentModelIndex currentIndex = ui->tvwChars->currentIndex();
    QPair<int, int> currentSelection(currentIndex.parent().row(), currentIndex.row());

    /* 'Adding font' policy: place font under current font; select it
     */

    // Add new font to object
    if (_font->fonts().isEmpty()) // font file is empty
        _font->appendFont(new SingleFont());
    else if (!_font->selection().contains(FontCollection::CharacterSelection)) // font is primarily selected
        _font->insertFont(currentSelection.second + 1, new SingleFont());
    else // character is primarily selected
        _font->insertFont(currentSelection.first + 1, new SingleFont());

    emit setModified(true);
    reloadUi();
    switchSelection();

    // Control current selection
    if (!_font->selection().contains(FontCollection::CharacterSelection)) // font is primarily selected
        ui->tvwChars->setCurrentIndex(model->index(currentSelection.second + 1, 0));
    else // character is primarily selected
        ui->tvwChars->setCurrentIndex(model->index(currentSelection.first + 1, 0));

    ui->stwProperties->setCurrentIndex(0);
    ui->ledFontName->setFocus();
}

void wdgEditor::addCharacter()
{
    if (!_font->selection().contains(FontCollection::FontSelection)) return;

    int prevPixelRow = ui->sbxHighestPixelInFontRow->value();

    QPersistentModelIndex currentIndex = ui->tvwChars->currentIndex();
    QPair<int, int> currentSelection(currentIndex.parent().row(), currentIndex.row());

    /* 'Adding characters' policy - if selection is...
     *  - a font: place character on bottom
     *  - a character: place character under current character
     */

    // Add new char to object
    if (!_font->selection().contains(FontCollection::CharacterSelection)) // font is primarily selected
        _font->fonts().at(_font->selection().value(FontCollection::FontSelection))->appendCharacter(new Character());
    else // character is primarily selected
        _font->fonts().at(_font->selection().value(FontCollection::FontSelection))->insertCharacter(_font->selection().value(FontCollection::CharacterSelection) + 1, new Character());

    emit setModified(true);
    reloadUi();
    switchSelection();

    // Control current selection
    if (!_font->selection().contains(FontCollection::CharacterSelection)) // font is primarily selected
        ui->tvwChars->setCurrentIndex(model->index(_font->fonts().at(currentSelection.second)->characters().count() - 1, 0, model->index(currentSelection.second, 0)));
    else // character is primarily selected
        ui->tvwChars->setCurrentIndex(model->index(_font->selection().value(FontCollection::CharacterSelection) + 1, 0, model->index(currentSelection.first, 0)));

    ui->stwProperties->setCurrentIndex(1);
    ui->ledCharacter->setFocus();

    if (set.read(objectName(), "keepPixelRow").toBool()) ui->sbxHighestPixelInFontRow->setValue(prevPixelRow);
}

void wdgEditor::deleteItem()
{
    if (_font->selection().contains(FontCollection::CharacterSelection))
    {
        _font->fonts().at(_font->selection().value(FontCollection::FontSelection))->characters().removeAt(_font->selection().value(FontCollection::CharacterSelection));

        if (_font->fonts().at(_font->selection().value(FontCollection::FontSelection))->characters().isEmpty())
            ui->tvwChars->setCurrentIndex(ui->tvwChars->currentIndex().parent());
    }
    else if (_font->selection().contains(FontCollection::FontSelection))
    {
        _font->removeFont(_font->selection().value(FontCollection::FontSelection));
        ui->tvwChars->setExpanded(model->index(_font->selection().value(FontCollection::FontSelection), 0), false);
    }
    else return;

    reloadUi();
    switchSelection();

    emit setModified(true);
}

void wdgEditor::search() // TODO
{
    qInfo() << "bla";
    if (!ui->fraSearch->isVisible())
    {
        ui->fraSearch->setVisible(true);
        ui->ledSearch->selectAll();
        return;
    }

    if (!_font->selection().contains(FontCollection::CharacterSelection)) return;
    currentSearch = ui->ledSearch->text();

    qDebug().noquote() << "Find char: '" + currentSearch + "'";

    actionGoToNextError->setEnabled(true);

    // Search for char
    ui->tvwChars->setCurrentIndex(model->index(0, 0));

    for (int i = 0; i < _font->fonts().at(_font->selection().value(FontCollection::FontSelection))->characters().count(); i++)
    {
        if (_font->fonts().at(_font->selection().value(FontCollection::FontSelection))->characters().at(i)->character() == currentSearch)
        {
            switchSelection();
            return;
        }

        ui->tvwChars->setCurrentIndex(model->index(i + 1, 0));
    }

    QMessageBox::information(this, tr("Character not found"), tr("The entered character could not be found."));
    qDebug() << "Character not found.";
    currentSearch.clear();
    actionGoToNextError->setEnabled(false);
}

void wdgEditor::goToNextError() // TODO
{
    if (!_font->selection().contains(FontCollection::CharacterSelection)) return;
    qDebug() << "Go to next search result";
    if (currentSearch != "")
    {
        if (_font->fonts().at(_font->selection().value(FontCollection::FontSelection))->characters().count() == 1)
            return;

        int i = ui->tvwChars->currentIndex().row();

        QList<Character*> tempList;
        tempList = _font->fonts().at(_font->selection().value(FontCollection::FontSelection))->characters();
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
            else if (tempList.at(i)->character() == currentSearch)
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

void wdgEditor::checkCharValidity()
{
    if (!_font->selection().contains(FontCollection::CharacterSelection)) return; // TODO: Check of all characters (with in-TreeView stylesheeting)

    ui->lblCharacter->setStyleSheet("");
    ui->lblRightPixel->setStyleSheet("");
    ui->lblLeftPixel->setStyleSheet("");
    ui->lblHighestPixelInFontRow->setStyleSheet("");

    if (!_font->selection().contains(FontCollection::CharacterSelection) || _font->fonts().at(_font->selection().value(FontCollection::FontSelection))->characters().isEmpty()) return;

    Character *character = _font->fonts().at(_font->selection().value(FontCollection::FontSelection))->characters().at(_font->selection().value(FontCollection::CharacterSelection));

    if (character->character().isEmpty()) ui->lblCharacter->setStyleSheet("color:red");
    else
    {
        QStringList tempCharList;
        foreach (Character *current, _font->fonts().at(_font->selection().value(FontCollection::FontSelection))->characters())
            tempCharList << current->character();

        if (tempCharList.indexOf(character->character()) != -1)
        {
            tempCharList.removeAt(tempCharList.indexOf(character->character()));
            if (tempCharList.indexOf(character->character()) != -1)
                ui->lblCharacter->setStyleSheet("color:red");
        }
    }

    if (character->leftPixel() > character->rightPixel())
    {
        ui->lblRightPixel->setStyleSheet("color:goldenrod");
        ui->lblLeftPixel->setStyleSheet("color:goldenrod");
    }

    if (character->rightPixel() == -1) ui->lblRightPixel->setStyleSheet("color:red");
    if (character->leftPixel() == -1) ui->lblLeftPixel->setStyleSheet("color:red");
    if (character->highestPixelInFontRow() == -1) ui->lblHighestPixelInFontRow->setStyleSheet("color:red");

    if (QFile(set.read("main", "mainDir").toString() + "/Fonts/" + _font->fonts().at(_font->selection().value(FontCollection::FontSelection))->alphaTexture()).exists())
    {
        QImage alphaTexture(set.read("main", "mainDir").toString() + "/Fonts/" + _font->fonts().at(_font->selection().value(FontCollection::FontSelection))->alphaTexture());

        if (alphaTexture.width() != 0 || alphaTexture.height() != 0)
        {
            if (character->rightPixel() > QString::number(alphaTexture.width()).toInt())
                ui->lblRightPixel->setStyleSheet("color:red");

            if (character->leftPixel() > QString::number(alphaTexture.width()).toInt())
                ui->lblLeftPixel->setStyleSheet("color:red");

            if (character->highestPixelInFontRow() > QString::number(alphaTexture.width()).toInt())
                ui->lblHighestPixelInFontRow->setStyleSheet("color:red");
        }
    }
}

void wdgEditor::checkPropValidity()
{
    if (!_font->selection().contains(FontCollection::FontSelection)) return;

    ui->lblFontName->setStyleSheet("");
    ui->lblColorTexture->setStyleSheet("");
    ui->lblAlphaTexture->setStyleSheet("");
    ui->lblMaxHeigthOfChars->setStyleSheet("");
    ui->lblDistanceBetweenChars->setStyleSheet("");

    if (_font->fonts().at(_font->selection().value(FontCollection::FontSelection))->name().isEmpty())
        ui->lblFontName->setStyleSheet("color:red");

    if (!_font->fonts().at(_font->selection().value(FontCollection::FontSelection))->colorTexture().isEmpty() && !QFile(set.read("main", "mainDir").toString() + "/Fonts/" + _font->fonts().at(_font->selection().value(FontCollection::FontSelection))->colorTexture()).exists())
        ui->lblColorTexture->setStyleSheet("color:red");

    if (_font->fonts().at(_font->selection().value(FontCollection::FontSelection))->alphaTexture().isEmpty() || !QFile(set.read("main", "mainDir").toString() + "/Fonts/" + _font->fonts().at(_font->selection().value(FontCollection::FontSelection))->alphaTexture()).exists())
        ui->lblAlphaTexture->setStyleSheet("color:red");

    if (ui->sbxMaxHeigthOfChars->text().isEmpty() || (_font->fonts().at(_font->selection().value(FontCollection::FontSelection))->maxHeightOfChars() == 0))
        ui->lblMaxHeigthOfChars->setStyleSheet("color:red");

    if (ui->sbxDistanceBetweenChars->text().isEmpty())
        ui->lblDistanceBetweenChars->setStyleSheet("color:red");

    if (QFile(set.read("main", "mainDir").toString() + "/Fonts/" + _font->fonts().at(_font->selection().value(FontCollection::FontSelection))->alphaTexture()).exists())
    {
        QImage alphaTexture(set.read("main", "mainDir").toString() + "/Fonts/" + _font->fonts().at(_font->selection().value(FontCollection::FontSelection))->alphaTexture());

        if (alphaTexture.width() != 0 || alphaTexture.height() != 0)
        {
            if (_font->fonts().at(_font->selection().value(FontCollection::FontSelection))->maxHeightOfChars() > QString::number(alphaTexture.height()).toInt())
                ui->lblMaxHeigthOfChars->setStyleSheet("color:red");

            if (_font->fonts().at(_font->selection().value(FontCollection::FontSelection))->distanceBetweenChars() > QString::number(alphaTexture.width()).toInt())
                ui->lblDistanceBetweenChars->setStyleSheet("color:red");
        }
    }
}

void wdgEditor::moveChar(int sel, Move action)
{
    if (!_font->selection().contains(FontCollection::FontSelection)) return;

    if (action == Move::Up && sel == 0) return;
    if (action == Move::Down && _font->selection().contains(FontCollection::CharacterSelection) && sel > (_font->fonts().at(_font->selection().value(FontCollection::FontSelection))->characters().count() - 1)) return;
    if (action == Move::Down && _font->selection().contains(FontCollection::CharacterSelection) && sel > (_font->fonts().count() - 1)) return;

    int moving = action == Move::Up ? sel - 1 : sel + 1;

    if (_font->selection().contains(FontCollection::CharacterSelection))
        _font->fonts().at(_font->selection().value(FontCollection::FontSelection))->moveCharacter(sel, moving);
    else if (_font->selection().contains(FontCollection::FontSelection))
        _font->moveFont(sel, moving);
    else
        return;

    emit setModified(true);
    reloadUi();

    if (_font->selection().contains(FontCollection::CharacterSelection)) ui->tvwChars->setCurrentIndex(model->index(moving, 0, ui->tvwChars->currentIndex().parent()));
    else if (_font->selection().contains(FontCollection::FontSelection)) ui->tvwChars->setCurrentIndex(model->index(moving, 0));
}

// Reloads TreeView
void wdgEditor::reloadUi()
{
    int currentScrollbarPosition = ui->tvwChars->verticalScrollBar()->value();
    QPersistentModelIndex currentIndex = ui->tvwChars->currentIndex();
    QPair<int, int> currentSelection(currentIndex.parent().row(), currentIndex.row());

    fontExpansions.clear(); // TODO: clear on font reload

    for (int i = 0; i < model->rowCount(); i++)
        fontExpansions << ui->tvwChars->isExpanded(model->index(i, 0)); // TODO: implement for delete, move!

    model->clear();

    for (int i = 0; i < _font->fonts().count(); i++)
    {
        SingleFont *font = _font->fonts().at(i);

        QStandardItem *fontItem = new QStandardItem(font->name().isEmpty() ? QString("(%1)").arg(tr("unnamed")) : font->name());
        fontItem->setRowCount(font->characters().count());

        for (int j = 0; j < font->characters().count(); j++) fontItem->setChild(j, 0, new QStandardItem(font->characters().at(j)->character().isEmpty() ? QString("(%1)").arg(tr("undefined")) : font->characters().at(j)->character()));

        model->invisibleRootItem()->appendRow(fontItem);
    }

    // Re-set expansions
    for (int i = 0; i < fontExpansions.count(); i++) ui->tvwChars->setExpanded(model->index(i, 0), fontExpansions[i]);

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

    ui->lblStatistics->setText(tr("%n font(s)", "", _font->fonts().count()) + ", " + tr("%n character(s) total", "", _font->totalCharacterCount()));

    // checkCharValidity();
    // checkPropValidity();

    actionDeleteItem->setDisabled(model->rowCount() == 0);

    actionMoveUp->setEnabled(ui->tvwChars->currentIndex().row() > 0);
    if (_font->selection().contains(FontCollection::CharacterSelection))
        actionMoveDown->setEnabled(ui->tvwChars->currentIndex().row() < (_font->fonts().at(ui->tvwChars->currentIndex().parent().row())->characters().count() - 1));
    else if (_font->selection().contains(FontCollection::FontSelection))
        actionMoveDown->setEnabled(ui->tvwChars->currentIndex().row() < (_font->fonts().count() - 1));
    else
        actionMoveDown->setEnabled(false);

    actionGoToNextError->setDisabled(_font->fonts().isEmpty());

    // TODO FOR Char position preview: Reload tex preview here (maybe in thread)
}

void wdgEditor::on_ledFontName_textChanged(const QString &arg1)
{
    if (!_font->selection().contains(FontCollection::FontSelection)) return;
    if (_font->selection().value(FontCollection::FontSelection) == -1) return;

    _font->fonts().at(_font->selection().value(FontCollection::FontSelection))->setName(arg1);

    reloadUi();
    checkPropValidity();
    checkCharValidity();
    emit setModified(true);
}

void wdgEditor::on_ledColorTexture_textChanged(const QString &arg1)
{
    if (!_font->selection().contains(FontCollection::FontSelection)) return;
    if (_font->selection().value(FontCollection::FontSelection) == -1) return;

    _font->fonts().at(_font->selection().value(FontCollection::FontSelection))->setColorTexture(arg1);

    reloadUi();
    checkPropValidity();
    checkCharValidity();
    emit setModified(true);

    emit reloadPreview();
}

void wdgEditor::on_btnColorTexture_clicked()
{
    if (!_font->selection().contains(FontCollection::FontSelection)) return;
    if (_font->selection().value(FontCollection::FontSelection) == -1) return;

    QString filename = QFileDialog::getOpenFileName(this, tr("Select color texture..."), set.read("main", "mainDir").toString() + "/Fonts", tr("Bitmap picture") + " (*.bmp)");

    if (filename.isEmpty()) return;

    _font->fonts().at(_font->selection().value(FontCollection::FontSelection))->setColorTexture(filename.remove(0, QString(set.read("main", "mainDir").toString() + "/Fonts").size() + 1));

    if (ui->ledColorTexture->text() != _font->fonts().at(_font->selection().value(FontCollection::FontSelection))->colorTexture())
        qDebug() << QString("New color texture: '%1'").arg(filename);

    ui->ledColorTexture->setText(_font->fonts().at(_font->selection().value(FontCollection::FontSelection))->colorTexture());
}

void wdgEditor::on_ledAlphaTexture_textChanged(const QString &arg1)
{
    if (!_font->selection().contains(FontCollection::FontSelection)) return;
    if (_font->selection().value(FontCollection::FontSelection) == -1) return;

    _font->fonts().at(_font->selection().value(FontCollection::FontSelection))->setAlphaTexture(arg1);

    reloadUi();
    checkPropValidity();
    checkCharValidity();
    emit setModified(true);

    emit reloadPreview();
}

void wdgEditor::on_btnAlphaTexture_clicked()
{
    if (!_font->selection().contains(FontCollection::FontSelection)) return;
    if (_font->selection().value(FontCollection::FontSelection) == -1) return;

    QString filename = QFileDialog::getOpenFileName(this, tr("Select alpha texture..."), set.read("main", "mainDir").toString() + "/Fonts", tr("Bitmap picture") + " (*.bmp)");
    if (filename.isEmpty()) return;

    _font->fonts().at(_font->selection().value(FontCollection::FontSelection))->setAlphaTexture(filename.remove(0, QString(set.read("main", "mainDir").toString() + "/Fonts").size() + 1));

    if (ui->ledAlphaTexture->text() != _font->fonts().at(_font->selection().value(FontCollection::FontSelection))->alphaTexture())
        qDebug() << QString("New alpha texture: '%1'").arg(filename);

    ui->ledAlphaTexture->setText(_font->fonts().at(_font->selection().value(FontCollection::FontSelection))->alphaTexture());
}

void wdgEditor::on_sbxMaxHeigthOfChars_valueChanged(int arg1)
{
    if (!_font->selection().contains(FontCollection::FontSelection)) return;
    if (_font->selection().value(FontCollection::FontSelection) == -1) return;

    _font->fonts().at(_font->selection().value(FontCollection::FontSelection))->setMaxHeightOfChars(arg1);

    reloadUi();
    checkPropValidity();
    checkCharValidity();
    emit setModified(true);
}

void wdgEditor::on_sbxDistanceBetweenChars_valueChanged(int arg1)
{
    if (!_font->selection().contains(FontCollection::FontSelection)) return;
    if (_font->selection().value(FontCollection::FontSelection) == -1) return;

    _font->fonts().at(_font->selection().value(FontCollection::FontSelection))->setDistanceBetweenChars(arg1);

    reloadUi();
    checkPropValidity();
    checkCharValidity();
    emit setModified(true);
}

void wdgEditor::on_btnCloseSearch_clicked()
{
    ui->fraSearch->setVisible(false);
}
