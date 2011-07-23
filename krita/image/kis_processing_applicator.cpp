/*
 *  Copyright (c) 2011 Dmitry Kazakov <dimula73@gmail.com>
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

#include "kis_processing_applicator.h"

#include "kis_image.h"
#include "kis_node.h"
#include "kis_processing_visitor.h"
#include "commands/kis_processing_command.h"
#include "kis_stroke_strategy_undo_command_based.h"


class UpdateCommand : public KUndo2Command
{
public:
    UpdateCommand(KisImageWSP image, KisNodeSP node,
                  bool recursive, bool finalUpdate)
        : m_image(image),
          m_node(node),
          m_recursive(recursive),
          m_finalUpdate(finalUpdate)
    {
    }

    void redo() {
        if(m_finalUpdate) doUpdate();
    }

    void undo() {
        if(!m_finalUpdate) doUpdate();
    }

private:
    void doUpdate() {
        if(m_recursive) {
            m_image->refreshGraphAsync(m_node);
        }

        m_node->setDirty(m_image->bounds());
    }

private:
    KisImageWSP m_image;
    KisNodeSP m_node;
    bool m_recursive;
    bool m_finalUpdate;
};


KisProcessingApplicator::KisProcessingApplicator(KisImageWSP image,
                                                 KisNodeSP node,
                                                 bool recursive,
                                                 const QString &name)
    : m_image(image),
      m_node(node),
      m_recursive(recursive)
{
    KisStrokeStrategyUndoCommandBased *strategy =
        new KisStrokeStrategyUndoCommandBased(name, false,
                                              m_image->realUndoAdapter());

    m_strokeId = m_image->startStroke(strategy);
    applyCommand(new UpdateCommand(m_image, m_node, m_recursive, false));
}

KisProcessingApplicator::~KisProcessingApplicator()
{
}


void KisProcessingApplicator::applyVisitor(KisProcessingVisitorSP visitor,
                                           KisStrokeJobData::Sequentiality sequentiality,
                                           KisStrokeJobData::Exclusivity exclusivity)
{
    if(!m_recursive) {
        applyCommand(new KisProcessingCommand(visitor, m_node),
                     sequentiality, exclusivity);
    }
    else {
        visitRecursively(m_node, visitor, sequentiality, exclusivity);
    }
}

void KisProcessingApplicator::visitRecursively(KisNodeSP node,
                                               KisProcessingVisitorSP visitor,
                                               KisStrokeJobData::Sequentiality sequentiality,
                                               KisStrokeJobData::Exclusivity exclusivity)
{
    // simple tail-recursive iteration

    KisNodeSP nextNode = node->firstChild();
    while(nextNode) {
        visitRecursively(nextNode, visitor, sequentiality, exclusivity);
        nextNode = nextNode->nextSibling();
    }

    applyCommand(new KisProcessingCommand(visitor, node),
                 sequentiality, exclusivity);
}

void KisProcessingApplicator::applyCommand(KUndo2Command *command,
                                           KisStrokeJobData::Sequentiality sequentiality,
                                           KisStrokeJobData::Exclusivity exclusivity)
{
    m_image->addJob(m_strokeId,
                    new KisStrokeStrategyUndoCommandBased::
                    Data(KUndo2CommandSP(command),
                         sequentiality,
                         exclusivity));
}

void KisProcessingApplicator::end()
{
    applyCommand(new UpdateCommand(m_image, m_node, m_recursive, true));
    m_image->endStroke(m_strokeId);
}

