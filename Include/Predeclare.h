#pragma once

//#define RUN



namespace Quad
{

//window id占쏙옙 占쏙옙占쏙옙占싹댐옙. 占쏙옙占쏙옙占싹놂옙占쏙옙 window占싸쏙옙占싹쏙옙占쏙옙 占싹놂옙占쏙옙 id占쏙옙 占쏙옙占쏙옙占쏙옙占싼댐옙.
#define GAMEWINDOW 0
#define ATTRIBUTEWINDOW 1
#define FILEUIWINDOW 2
#define FRAMEWINDOW 3
#define DRAGANDDROPWINDOW 4
#define POPUPWINDOW 5


#define WINDOWMAXNUM 6

	class Object;
	class Entity;
	class UiEntity;
	class FileUiEntity;
	class DirectoryUiEntity;
	class Texture;
	class Camera;
	class BoundingVolume;
	class BoundingBox;
	class BoundingOrientedBox;
	class BoundingSphere;
	class BoundingVolumeGenerator;
	class CollisionHelper;
	class ResourceController;
	class MapController;
	class GamePlayWindowController;
	class Controller;
	class DockingWindowController;
	class DescriptorHeapManagerMaster;
	class DescriptorHeapManager;
	class CbvSrvUavHeapManager;
	class DsvHeapManager;
	class RtvHeapManager;
	class SamplerHeapManager;
	class Event;
	class EventTable;
	class GamePlayEvent;
	class MouseEvent;
	class GeometryGenerator;
	class MaterialGenerator;
	class TextureLoader;
	class ResourceLoader;
	class System;
	class GamePlaySystem;
	class GamePlayUiSystem;
	class AttributeSystem;
	class FileUISystem;
	struct RenderSettingItem;
	struct RenderItem;
	class GraphicPipeLine;
	class TextureManager;
	class MeshManager;
	class MaterialManager;
	class MapManager;
	class ConstantBufferManager;
	class Map;
	class SceneGraph;

	class QuadTree;
	class Material;
	class Mesh;
	class Model;
	class State;
	class SystemState;
	class GamePlayUiGamePlayState;
	class DefaultState;
	class GamePlayUiGamePlaySelectState;
	class MouseClickState;
	class ResourceStorer;
	class UploadBuffer;
	class ReaderWriter;
	template<typename T> class CircleQueue;
	class TaskWindow;
	class GameRenderWindow;
	class FileUiWindow;
	class Ray;
	class MathHelper;
	class Utility;
	template<typename T> class Singleton;
	class GameTimer;
	class GraphicCommand;
	class DoException;


	template<typename T> class PoolAllocator;
	template<typename T> class DestructingPoolAllocator;

	struct Attribute;
	struct RawSchemaData;

	class WorldEditUiEntity;
	class WorldEditEntity;
	class AttributeWindow;

	class UiSystem;
	class AttributeUiSystem;
	class RenderSystem;

	class Text;
	class TextRun;
	class TextItem;
	class Character;
	class TextCursor;
	class TextSelectBox;
	class Font;

	class KeyEvent;
	class SystemSelectState;
	class TextFactory;


	class KeyBoard;
	using VK_KEY = unsigned char;
	using KEY_DOWN_FLAG = bool;
	using UNICODE_VALUE = wchar_t;

#define KEY_DOWN true
#define KEY_UP false


	class DeleteTextureEvent;

	class BoundaryRect;
	class ResizeHandle;

	class MouseClickSelectEvent;
	class BoundingVolumeComplex;

	class BoundaryStick;
	
	class ResizeHandleEvent;
	class BoxEntity;
	class Vector3Box;

	class InstancingTable;
	class StructuredBuffer;

	struct InstancingData;
	//struct RenderSystem::InstancingRenderItem;
	//struct RenderSystem::ModelInstancingData;


	class Effect;
	class RenderPass;
	class RootSignature;

	class EffectLoader;

	struct RawRenderPassData;
	struct RawEffectData;
	struct RawShaderResourceData;

	class StructuredBufferFactory;
	class ConstantBuffer;
	class ShaderResourceStructuredBuffer;
	class ShaderResourceFactory;
	class ShaderResourceTable;

	class ShaderResourceViewConvert;
	class ShaderResourceTypeIDConverter;
	class ShaderResourceRegisterConverter;
	class InputLayoutTable;
	class EffectUtility;

	enum class ShaderResourceViewID
	{
		eNone = 0,
		eCBV = 1,
		eSRV = 2,
		eSPV = 5
		
	};


	class MapGraph;
	class SkinningMesh;
	class StaticMesh;
	class FbxParser;
	class ButtonUiEntity;


	class Light;
	struct LightItem;
	class WindowChromeSystem;
	class TitleBarEntity;

	enum class ESystemType
	{
		eMainSystem = 0,
		//eUiSystem,
		eWindowLayoutSystem,
		eDockingSystem,
		eDummySystem
	};

	enum class ESystemID
	{
		eNone =-1,

		eGamePlayMainSystem = 0,
		//eGamePlayUiSystem,
		
		eGamePlayLayoutSystem,
		eGamePlayDockingSystem,

		eAttributeMainSystem,
		//eAttributeUiSystem,
		eAttributeLayoutSystem,
		eAttributeDockingSystem,

		eFileUiMainSystem,
		//eFileUiUiSystem,
		eFileUiLayoutSystem,
		eFileUiDockingSystem,

		eFrameMainSystem,
		//eFrameUiSystem,
		eFrameLayoutSystem,
		eFrameDockingSystem,

		eDragAndDropMainSystem,
		//eDragAndDropUiSystem,
		eDragAndDropLayoutSystem,
		eDragAndDropDockingSystem,

		ePopupUiSystem

	};

	


	class WindowRequestEvent;
	enum class EWindowRequestEventType
	{
		eExitProgram = 0,//占쏙옙占쏙옙占쏙옙 占쏙옙占싸그뤄옙 占쏙옙占쏙옙
		eSetWindowPosition,
		eWindowMove,
		eWindowMoveEnd,
		eWindowEnd,//占싱거댐옙 창占쏙옙 占쏙옙占쌩곤옙 占쌥는곤옙
		eWindowMaximize,
		eWindowMinimize
	};

	class WindowResizeEvent;
	class WindowCloseButton;
	class WindowControlEntity;
	class Mouse;
	class WindowButton;
	class FrameWindow;
	class FrameWindowController;

	class FrameWindowUiSystem;
	class FrameWindowSystem;

	class FileUiUiSystem;

#define DEFAULT_WINDOW_WIDTH 600
#define DEFAULT_WINDOW_HEIGHT 400

	enum class EWindowEdgeDragResizableDirection
	{
		eNone=0,
		eLeft,
		eRight,
		eTop,
		eBottom

	};
	class ChromeSystemCamera;
	class MaxRestoreWindowButton;
	class WindowMinButton;
	class MouseInOutEvent;


	enum class EWindowShowState
	{
		eNormal=0,		
		eMaximize,
		eMinimize,
		
	};
	class WindowDockingSystem;
	class DockingSelectBox;
	class DockingSystemCamera;

	class DockingPanel;

	enum class EDockingDirection
	{
		eLeft = 0,
		eRight,
		eTop,
		eBottom
	};

	class SizeControllerStickEntity;
	class FrameWindowDockingSystem;
	class ChildWindowDockingSystem;

	enum class ESizeControlStickDirection
	{
		eHorizontal=0,
		eVertical
	};

	class FileUiWindowContoller;
	class LoadRequestFileEvent;
	class LoadFileResponseEvent;
	class Asset;

	enum class EAssetType
	{
		eMesh=0,
		eMaterial,
		eTexture,
		eLine,
		eAnimationClip,
		eSkeleton,
		eFolder
	};

	class ContentItemUiEntity;
	class LogicalContentItemUiEntityFolder;
	class ContentItem;

	enum class EContentItemType
	{
		eAsset=0,
		eLogicalFolder,
		eProject
	};
	class FileUiSystemCamera;
	class FastAccessContainer;
	class UnOrderedMapContainer;
	class VectorContainer;
	class PanelUiEntity;
	class ContentItemImageUiEntity;


	enum class EUiEntityMouseState
	{
		eDefault=0,
		eHover,
		eHoverSelect,
		eNotHoverSelect
	};

	class DragAndDropWindow;
	class DragAndDropWindowController;
	class DummySystem;
	class BaseWindow;
	class DragAndDropSystem;
	class DragAndDropUiEntity;

	//class ContentItemUiEntityFactory;
	class Grid;
	class Line;
	class LineManager;
	class ShaderManager;
	class Shader;
	class RenderPassTwo;

	class ShaderResource;
	class ShaderResourceConstantBuffer;
	class ShaderResourceSampler;
	class ShaderResourceTextureTT;

	struct PassData;
	class SamplerManager;
	class PopupWindow;
	class LineMesh;

	class Skeleton;
	class Bone;
	class AnimationClip;
	struct AnimationKeyFrame;
	class BoneAnimationClip;
	class AnimationClipManager;
	class SkeletonManager;
	class AnimationComponent;

	class RenderPassSystem;
	class RenderPassTwo;
	struct RenderPassItem;




	enum ERenderPassID
	{
		eShadowMap,
		eOpaque,
		eTransparent,
		eGizmo,


	};


	class LineFactory;
	class LineBase;

	class Collider;
	class BoxCollider;
	class SphereCollider;
	class FrustumCollider;
	class OrientedBoxCollider;
	class CameraBoxCollider;
	class CameraFrustumCollider;

	class CollisionWorld;
	class ColliderGenerator;
	class BoundingFrustum;
	class PointLightCollider;
	class Gizmo;
	class GizmoHandle;
	class GizmoRotationHandle;

	class LineBaseMaterial;

	class EventDispatcher;

	class StateComponent;

	enum class EAxisType
	{
		eX = 0,
		eY,
		eZ
	};

	class GamePlayWindowCamera;

	class GizmoScaleHandle;
	class NotifyDragAndDropEvent;
	class GamePlaySystemDefaultState;
	class ScrollBarEntity;
	class FolderListPanelEntity;
	class TextPanelEntity;


	struct LogicalContentItemUiEntityFolderNode;
	class ListExpandButton;
	class FolderPanelEntity;
	class IconEntity;
	class PopupWindowUiSystem;

	class PopupWindowController;
	class PopupSystemCamera;
	class ContentItemUiPanelEntity;
	class NavigationFolderPanelEntity;
	
	class TextBox;
	class TextCharacterPanelEntity;
	class TextCharacterEntity;
	class TextCursorEntity;


	enum class ETextAlignmentDirection
	{
		eLeft=0,
		eCenter,
		eRight

	};

	class WindowActiveEvent;
	class TransformPropertyPanel;
	class GameObjectSelectEvent;
	class AssetImageFrameEntity;

	template<typename valueType>
	class HandleTable;

	class ScrollListPanel;
	class TextBoxPanel;
	class ObjectIDManager;
	class SubMeshListPanel;



	class AttrUiSystemSelectObjectComponent;
	class ComponentIDGenerator;
	class BaseComponent;
	

	class ExpandablePanel;
	class AnimationState;
	class GameBase;


	class EditObjectFactory;

	enum class EAnimationTimeMode
	{
		eFPS30 = 0,
		eFPS60
	};
	class AnimationClipSplitter;


	struct MapLayer;

	class RenderTargetTexture;
	class DepthStencilTexture;
	class TextureFactory;
	class UserCamera;

	class FrustumCamera;
	class OrthogoanlCamera;

	class SceneNode;
	class ObjectNode;
	class TransformNode;


	class SceneElement;
	class MeshComponent;
	class ColliderComponent;
	class AnimationComponent;

	class BaseObjectFactory;
	class BaseObjectManager;

	class RuntimeGameObjectManager;
	class EditGameObjectManager;

	class ContentItemBaseUiEntity;

	class CreatingProjectDirector;
	class ProjectDirector;
	class Project;
	class MapMetaData;


	class ListPanel;
	class BaseWindowController;
	class EngineModeDirector;
	class EditorModeDirector;

	class IProgramDirector;

	struct ShaderResourceStructTypeDesc;

	class BaseWindowMsgHandlerComponent;


	class ICollisionWorldFactoryImpl;
	
	class CollisionWorldFactory;


	class ComponentUpdateSystem;
	class AnimationUpdateSystem;

	class Spline;
	class UiCollider;
	class RectCollider;
	class SplineCollider;
	class UiColliderComponent;

	class UiCollisionWorld;
	class ISpacePartitioningStructureFactoryImpl;



}