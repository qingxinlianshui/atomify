import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import Atomify 1.0

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

import QtQuick.Scene3D 2.0

import SimVis 1.0
import ShaderNodes 1.0

import "../desktop" // TODO should be separate controllers for desktop and mobile

Entity {
    id : root

    property var colorTexture
    property var spheres
    property real width
    property real height

    Material {
        id: finalMaterial
        parameters : [
            Parameter { name: "colorTexture"; value : root.colorTexture }
        ]
        effect: Effect {
            techniques : [
                Technique {
                    filterKeys: FilterKey {
                        name: "renderingStyle"
                        value: "forward"
                    }
                    graphicsApiFilter {
                        api: GraphicsApiFilter.OpenGL
                        profile: GraphicsApiFilter.CoreProfile
                        majorVersion: 3
                        minorVersion: 2
                    }
                    renderPasses : RenderPass {
                        id: finalRenderPass
                        filterKeys : FilterKey { name : "pass"; value : "final" }
                        shaderProgram : ShaderProgram {
                            vertexShaderCode: "
#version 330

uniform highp mat4 modelMatrix;

in highp vec4 vertexPosition;
in highp vec2 vertexTexCoord;

out highp vec2 texCoord;

void main()
{
    texCoord = vertexTexCoord;
    texCoord.y = 1.0 - texCoord.y;
    gl_Position = modelMatrix * vertexPosition;
}
"
                            
                            fragmentShaderCode: "
#version 330
uniform highp sampler2D colorTexture;
uniform highp vec2 winSize;

in highp vec2 texCoord;
out highp vec4 fragColor;

void main()
{
    fragColor = texture(colorTexture, texCoord);
}
"
                        }
                    }
                }
            ]
        }
    }
    
    components: [
        //                finalLayer,
        quadMesh,
        quadTransform,
        finalMaterial
    ]
}
