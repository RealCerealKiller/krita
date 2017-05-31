/*
 *  Copyright (c) 2017 Dmitry Kazakov <dimula73@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "KoSvgText.h"
#include <SvgUtil.h>
#include <KoXmlReader.h>
#include <SvgLoadingContext.h>
#include <QDebug>
#include "kis_dom_utils.h"

#include <KoColorBackground.h>
#include <KoGradientBackground.h>
#include <KoVectorPatternBackground.h>
#include <KoShapeStroke.h>


namespace {

struct TextPropertiesStaticRegistrar {
    TextPropertiesStaticRegistrar() {
        qRegisterMetaType<KoSvgText::AutoValue>("KoSvgText::AutoValue");
        QMetaType::registerEqualsComparator<KoSvgText::AutoValue>();
        QMetaType::registerDebugStreamOperator<KoSvgText::AutoValue>();

        qRegisterMetaType<KoSvgText::BackgroundProperty>("KoSvgText::BackgroundProperty");
        QMetaType::registerEqualsComparator<KoSvgText::BackgroundProperty>();
        QMetaType::registerDebugStreamOperator<KoSvgText::BackgroundProperty>();

        qRegisterMetaType<KoSvgText::StrokeProperty>("KoSvgText::StrokeProperty");
        QMetaType::registerEqualsComparator<KoSvgText::StrokeProperty>();
        QMetaType::registerDebugStreamOperator<KoSvgText::StrokeProperty>();
    }
};

static TextPropertiesStaticRegistrar textPropertiesStaticRegistrar;

}

namespace KoSvgText {

AutoValue parseAutoValueX(const QString &value, const SvgLoadingContext &context, const QString &autoKeyword)
{
    return value == autoKeyword ? AutoValue() : SvgUtil::parseUnitX(context.currentGC(), value);
}

AutoValue parseAutoValueY(const QString &value, const SvgLoadingContext &context, const QString &autoKeyword)
{
    return value == autoKeyword ? AutoValue() : SvgUtil::parseUnitY(context.currentGC(), value);
}

AutoValue parseAutoValueXY(const QString &value, const SvgLoadingContext &context, const QString &autoKeyword)
{
    return value == autoKeyword ? AutoValue() : SvgUtil::parseUnitXY(context.currentGC(), value);
}

AutoValue parseAutoValueAngular(const QString &value, const SvgLoadingContext &context, const QString &autoKeyword)
{
    return value == autoKeyword ? AutoValue() : SvgUtil::parseUnitAngular(context.currentGC(), value);
}

WritingMode parseWritingMode(const QString &value) {
    return (value == "tb-rl" || value == "tb") ? TopToBottom :
           (value == "rl-tb" || value == "rl") ? RightToLeft :
           LeftToRight;
}

Direction parseDirection(const QString &value) {
    return value == "rtl" ? DirectionRightToLeft : DirectionLeftToRight;
}

UnicodeBidi parseUnicodeBidi(const QString &value)
{
    return value == "embed" ? BidiEmbed :
           value == "bidi-override" ? BidiOverride :
           BidiNormal;
}

TextAnchor parseTextAnchor(const QString &value)
{
    return value == "middle" ? AnchorMiddle :
           value == "end" ? AnchorEnd :
           AnchorStart;
}

DominantBaseline parseDominantBaseline(const QString &value)
{
    return value == "use-script" ? DominantBaselineUseScript :
           value == "no-change" ? DominantBaselineNoChange:
           value == "reset-size" ? DominantBaselineResetSize:
           value == "ideographic" ? DominantBaselineIdeographic :
           value == "alphabetic" ? DominantBaselineAlphabetic :
           value == "hanging" ? DominantBaselineHanging :
           value == "mathematical" ? DominantBaselineMathematical :
           value == "central" ? DominantBaselineCentral :
           value == "middle" ? DominantBaselineMiddle :
           value == "text-after-edge" ? DominantBaselineTextAfterEdge :
           value == "text-before-edge" ? DominantBaselineTextBeforeEdge :
           DominantBaselineAuto;
}

AlignmentBaseline parseAlignmentBaseline(const QString &value)
{
    return value == "baseline" ? AlignmentBaselineDominant :
           value == "ideographic" ? AlignmentBaselineIdeographic :
           value == "alphabetic" ? AlignmentBaselineAlphabetic :
           value == "hanging" ? AlignmentBaselineHanging :
           value == "mathematical" ? AlignmentBaselineMathematical :
           value == "central" ? AlignmentBaselineCentral :
           value == "middle" ? AlignmentBaselineMiddle :
           (value == "text-after-edge" || value == "after-edge") ? AlignmentBaselineTextAfterEdge :
           (value == "text-before-edge" || value == "before-edge") ? AlignmentBaselineTextBeforeEdge :
           AlignmentBaselineAuto;
}

BaselineShiftMode parseBaselineShiftMode(const QString &value)
{
    return value == "baseline" ? ShiftNone :
           value == "sub" ? ShiftSub :
           value == "super" ? ShiftSuper :
           ShiftPercentage;
}

LengthAdjust parseLengthAdjust(const QString &value)
{
    return value == "spacingAndGlyphs" ? LengthAdjustSpacingAndGlyphs : LengthAdjustSpacing;
}

QString writeAutoValue(const AutoValue &value, const QString &autoKeyword)
{
    return value.isAuto ? autoKeyword : KisDomUtils::toString(value.customValue);
}

QString writeWritingMode(WritingMode value)
{
    return value == TopToBottom ? "tb" : value == RightToLeft ? "rl" : "lr";
}

QString writeDirection(Direction value)
{
    return value == DirectionRightToLeft ? "rtl" : "ltr";
}

QString writeUnicodeBidi(UnicodeBidi value)
{
    return value == BidiEmbed ? "embed" : value == BidiOverride ? "bidi-override" : "normal";
}

QString writeTextAnchor(TextAnchor value)
{
    return value == AnchorEnd ? "end" : value == AnchorMiddle ? "middle" : "start";
}

QString writeDominantBaseline(DominantBaseline value)
{
    return value == DominantBaselineUseScript ? "use-script" :
           value == DominantBaselineNoChange ? "no-change" :
           value == DominantBaselineResetSize ? "reset-size" :
           value == DominantBaselineIdeographic ? "ideographic" :
           value == DominantBaselineAlphabetic ? "alphabetic" :
           value == DominantBaselineHanging ? "hanging" :
           value == DominantBaselineMathematical ? "mathematical" :
           value == DominantBaselineCentral ? "central" :
           value == DominantBaselineMiddle ? "middle" :
           value == DominantBaselineTextAfterEdge ? "text-after-edge" :
           value == DominantBaselineTextBeforeEdge ? "text-before-edge" :
           "auto";
}

QString writeAlignmentBaseline(AlignmentBaseline value)
{
    return value == AlignmentBaselineDominant ? "baseline" :
           value == AlignmentBaselineIdeographic ? "ideographic" :
           value == AlignmentBaselineAlphabetic ? "alphabetic" :
           value == AlignmentBaselineHanging ? "hanging" :
           value == AlignmentBaselineMathematical ? "mathematical" :
           value == AlignmentBaselineCentral ? "central" :
           value == AlignmentBaselineMiddle ? "middle" :
           value == AlignmentBaselineTextAfterEdge ? "text-after-edge" :
           value == AlignmentBaselineTextBeforeEdge ? "text-before-edge" :
           "auto";
}

QString writeBaselineShiftMode(BaselineShiftMode value, qreal portion)
{
    return value == ShiftNone ? "baseline" :
           value == ShiftSub ? "sub" :
           value == ShiftSuper ? "super" :
           SvgUtil::toPercentage(portion);
}

QString writeLengthAdjust(LengthAdjust value)
{
    return value == LengthAdjustSpacingAndGlyphs ? "spacingAndGlyphs" : "spacing";
}

QDebug operator<<(QDebug dbg, const KoSvgText::AutoValue &value)
{
    dbg.nospace() << (value.isAuto ? "auto" : QString::number(value.customValue));
    return dbg.space();
}

void CharTransformation::mergeInParentTransformation(const CharTransformation &t)
{
    if (!xPos && t.xPos) {
        xPos = *t.xPos;
    }

    if (!yPos && t.yPos) {
        yPos = *t.yPos;
    }

    if (!dxPos && t.dxPos) {
        dxPos = *t.dxPos;
    }

    if (!dyPos && t.dyPos) {
        dyPos = *t.dyPos;
    }

    if (!rotate && t.rotate) {
        rotate = *t.rotate;
    }
}

bool CharTransformation::isNull() const
{
    return !xPos && !yPos && !dxPos && !dyPos && !rotate;
}

QPointF CharTransformation::adjustedTextPos(const QPointF &pos) const
{
    QPointF result = pos;

    if (xPos) {
        result.rx() = *xPos;
    }

    if (yPos) {
        result.ry() = *yPos;
    }

    if (dxPos) {
        result.rx() += *dxPos;
    }

    if (dyPos) {
        result.ry() += *dyPos;
    }

    return result;
}

bool CharTransformation::startsNewChunk() const
{
    return xPos || yPos;
}

bool CharTransformation::hasRelativeOffset() const
{
    return dxPos || dyPos;
}

QPointF CharTransformation::absolutePos() const
{
    QPointF result;

    if (xPos) {
        result.rx() = *xPos;
    }

    if (yPos) {
        result.ry() = *yPos;
    }

    return result;
}

QPointF CharTransformation::adjustRelativePos(const QPointF &pos) const
{
    QPointF result = pos;

    if (dxPos) {
        result.rx() += *dxPos;
    }

    if (dyPos) {
        result.ry() += *dyPos;
    }

    return result;
}

bool CharTransformation::operator==(const CharTransformation &other) const {
    return
        xPos == other.xPos && yPos == other.yPos &&
        dxPos == other.dxPos && dyPos == other.dyPos &&
            rotate == other.rotate;
}

namespace {
QDebug addSeparator(QDebug dbg, bool hasPreviousContent) {
    return hasPreviousContent ? (dbg.nospace() << "; ") : dbg;
}
}

QDebug operator<<(QDebug dbg, const CharTransformation &t)
{
    dbg.nospace() << "CharTransformation(";

    bool hasContent = false;

    if (t.xPos) {
        dbg.nospace() << "xPos = " << *t.xPos;
        hasContent = true;
    }

    if (t.yPos) {
        dbg = addSeparator(dbg, hasContent);
        dbg.nospace() << "yPos = " << *t.yPos;
        hasContent = true;
    }

    if (t.dxPos) {
        dbg = addSeparator(dbg, hasContent);
        dbg.nospace() << "dxPos = " << *t.dxPos;
        hasContent = true;
    }

    if (t.dyPos) {
        dbg = addSeparator(dbg, hasContent);
        dbg.nospace() << "dyPos = " << *t.dyPos;
        hasContent = true;
    }

    if (t.rotate) {
        dbg = addSeparator(dbg, hasContent);
        dbg.nospace() << "rotate = " << *t.rotate;
        hasContent = true;
    }

    dbg.nospace() << ")";
    return dbg.space();
}



QDebug operator<<(QDebug dbg, const BackgroundProperty &prop)
{
    dbg.nospace() << "BackgroundProperty(";

    dbg.nospace() << prop.property.data();

    if (KoColorBackground *fill = dynamic_cast<KoColorBackground*>(prop.property.data())) {
        dbg.nospace() << ", color, " << fill->color();
    }

    if (KoGradientBackground *fill = dynamic_cast<KoGradientBackground*>(prop.property.data())) {
        dbg.nospace() << ", gradient, " << fill->gradient();
    }

    if (KoVectorPatternBackground *fill = dynamic_cast<KoVectorPatternBackground*>(prop.property.data())) {
        dbg.nospace() << ", pattern, num shapes: " << fill->shapes().size();
    }

    dbg.nospace() << ")";
    return dbg.space();
}

QDebug operator<<(QDebug dbg, const StrokeProperty &prop)
{
    dbg.nospace() << "StrokeProperty(";

    dbg.nospace() << prop.property.data();

    if (KoShapeStroke *stroke = dynamic_cast<KoShapeStroke*>(prop.property.data())) {
        dbg.nospace() << ", " << stroke->resultLinePen();
    }

    dbg.nospace() << ")";
    return dbg.space();
}

}

