#ifndef LAMMPSCONTROLLER_H
#define LAMMPSCONTROLLER_H
#include <mpi.h>
#include "lammps/lammps.h"
#include "lammps/compute.h"
#include "lammpsoutput.h"
#include "CPcompute.h"
#include <memory>
#include <QVector>
#include <QString>
#include <QMap>
using namespace LAMMPS_NS;
class LAMMPSController
{
private:
    struct State {
        bool paused = false;
        bool runCommandActive = false;
        bool preRunNeeded = true;
        bool allComputesAdded = true;
        bool outputNeedsUpdate = true;
        int  simulationSpeed = 1;
        unsigned int runCommandStart = 0;
        unsigned int runCommandEnd = 0;
    };

    State m_state;
    QVector<QString> m_commands;
    QMap<QString, CPCompute*> m_computes;
    LAMMPS *m_lammps = NULL;

    void processComputes();
    void updateOutput();
    void executeActiveRunCommand();
    int findComputeId(QString identifier);
    bool computeExists(QString identifier);
public:
    LammpsOutput output;

    LAMMPSController();
    ~LAMMPSController();

    // Getters/setters
    LAMMPS *lammps() const;
    void setLammps(LAMMPS *lammps);
    int  simulationSpeed() const;
    void setSimulationSpeed(int simulationSpeed);
    QMap<QString, CPCompute *> computes() const;
    void setComputes(const QMap<QString, CPCompute *> &computes);
    bool getPaused() const;
    void setPaused(bool value);

    // Actions
    void executeCommandInLAMMPS(QString command);
    void processCommand(QString command);
    void loadScriptFromFile(QString filename);
    void runScript(QString script);
    void reset();
    void tick();
};

#endif // LAMMPSCONTROLLER_H
