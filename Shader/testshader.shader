{
    "Name": "MyShader",
    "UserResource": {
        "PassBuffer": {
            "Type": "ConstantBuffer",
            "BufferID": 1
        },
        "_TexMap": {
            "Type": "Tex2D",
            "FORMAT": "R8G8B8A8_UNORM"
        },
        "_LinearSampler": {
            "Type": "Sampler"
        }
    },
    "DefaultShader": {
        "Shadow": "Default"
    },
    "UserShader": {
        "MainPass": {
            "OPAQUE": true,
            "InputType": 0,
            "Uses": ["_TexMap", "PassBuffer","_LinearSampler"],
            "HLSL": {
                "File": "BaseUI.hlsl",
                "VS":"VS",
                "PS":"PS"
            },
            "RenderState": {
                "CullMode": 0,
                "FillMode": 0,
                "DepthCompareMode": 1,
                "BlendMode": 0,
                "DepthWriteMode": 1
            }



        }
    }
}