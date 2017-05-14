import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1
import Atomify 1.0
import "qrc:/core"
import "qrc:/mobile/dashboard/controls"

Simulation {
    id: root
    name: "ZnOH2"
    description: "ZnOH2"
    initialCameraPosition: Qt.vector3d(0,0,20)
    initialUpVector: Qt.vector3d(0,-1,0)
    
    controllers: [
        Summary {
            system: root.system
        },
        Thermometer {
            xLabel: "Time [fs]"
            xRange: 250
        }
    ]
}
