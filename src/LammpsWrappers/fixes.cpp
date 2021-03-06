#include "fixes.h"
#include "../lammpscontroller.h"
#include "simulatorcontrols/cpfix.h"
#include "simulatorcontrols/cpfixindent.h"

Fixes::Fixes(QObject *parent) : QObject(parent)
{

}

void Fixes::add(QString identifier, LAMMPSController *lammpsController) {
    if(lammpsController->simulatorControls.contains(identifier)) {
        CPFix *fix =  qobject_cast<CPFix*>(lammpsController->simulatorControls[identifier]);
        m_data.push_back(fix);
        m_dataMap.insert(identifier, fix);
    } else {
        LAMMPS_NS::FixIndent *lmp_fix = dynamic_cast<LAMMPS_NS::FixIndent*>(lammpsController->findFixByIdentifier(identifier));
        CPFix *fix;
        if(lmp_fix) {
            fix = new CPFixIndent();
        } else {
            fix = new CPFix();
        }
        fix->setIdentifier(identifier);
        m_data.push_back(fix);
        m_dataMap.insert(identifier, fix);
    }
}

void Fixes::remove(QString identifier) {
    CPFix *fix = qobject_cast<CPFix*>(m_dataMap[identifier]);
    m_data.removeOne(fix);
    m_dataMap.remove(identifier);
    delete fix;
}

void Fixes::reset() {
    QList<QObject*> data = m_data;
    for(QObject *object : data) {
        CPFix *fix = qobject_cast<CPFix*>(object);
        remove(fix->identifier());
    }
    data.clear();
    setModel(QVariant::fromValue(m_data));
    setCount(0);
}

bool Fixes::addOrRemove(LAMMPSController *lammpsController)
{
    LAMMPS_NS::Modify *modify = lammpsController->lammps()->modify;
    bool anyChanges = false;
    for(int fixIndex=0; fixIndex<modify->nfix; fixIndex++) {

        LAMMPS_NS::Fix *fix = modify->fix[fixIndex];
        QString identifier = QString::fromUtf8(fix->id);

        if(!m_dataMap.contains(identifier)) {
            anyChanges = true;
            add(identifier, lammpsController);
        }
    }

    QList<CPFix*> fixesToBeRemoved;
    for(QObject *object : m_data) {
        CPFix *fix = qobject_cast<CPFix*>(object);
        if(!lammpsController->fixExists(fix->identifier())) {
            // Not in LAMMPS anymore, we should remove it too
            anyChanges = true;
            fixesToBeRemoved.push_back(fix);
        }
    }

    for(CPFix *fix : fixesToBeRemoved) {
        remove(fix->identifier());
    }
    return anyChanges;
}

void Fixes::synchronizeQML(LAMMPSController *lammpsController)
{
    if(!lammpsController->lammps()) return;
    bool anyChanges = addOrRemove(lammpsController);
    if(anyChanges) {
        setModel(QVariant::fromValue(m_data));
        setCount(m_data.count());
    }

    for(QObject *obj : m_data) {
        SimulatorControl *control = qobject_cast<SimulatorControl*>(obj);
        control->updateData1D();
        CPFixIndent *fixIndent = qobject_cast<CPFixIndent*>(obj);
        if(fixIndent) {
            if(fixIndent->hovered()) {
                setActiveFixIndent(fixIndent);
            }
        }
    }

    if(m_activeFixIndent && !m_activeFixIndent->hovered()) {
        setActiveFixIndent(nullptr);
    }

}

void Fixes::synchronize(LAMMPSController *lammpsController)
{
    if(!lammpsController->lammps()) return;

    for(QObject *object : m_data) {
        CPFix *fix = qobject_cast<CPFix*>(object);
        fix->copyData(lammpsController);
    }
}

void Fixes::updateThreadOnDataObjects(QThread *thread) {
    for(QObject *obj : m_data) {
        SimulatorControl *control = qobject_cast<SimulatorControl*>(obj);
        for(QVariant &variant : control->data1D()) {
            Data1D *data = variant.value<Data1D *>();
            if(data->thread() != thread) {
                data->moveToThread(thread);
            }
        }
    }
}

CPFixIndent *Fixes::activeFixIndent() const
{
    return m_activeFixIndent;
}

QVector<SimulatorControl *> Fixes::simulatorControls()
{
    QVector<SimulatorControl*> controls;
    for(QObject *object : m_data) {
        SimulatorControl *control = qobject_cast<SimulatorControl*>(object);
        controls.append(control);
    }
    return controls;
}

int Fixes::count() const
{
    return m_count;
}

QVariant Fixes::model() const
{
    return m_model;
}

void Fixes::setModel(QVariant model)
{
    if (m_model == model)
        return;

    m_model = model;
    emit modelChanged(model);
}

void Fixes::setCount(int count)
{
    if (m_count == count)
        return;

    m_count = count;
    emit countChanged(count);
}

void Fixes::setActiveFixIndent(CPFixIndent *activeFixIndent)
{
    if (m_activeFixIndent == activeFixIndent)
        return;

    m_activeFixIndent = activeFixIndent;
    emit activeFixIndentChanged(m_activeFixIndent);
}
