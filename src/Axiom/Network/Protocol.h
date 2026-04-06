#pragma once

#include "Axiom/Core/Base.h"

#include <cstdint>

namespace Axiom {

	constexpr int32_t PROTOCOL_VERSION = 775;
	constexpr const char* MINECRAFT_VERSION = "26.1";

	enum class ConnectionState {
		Handshake,
		Status,
		Login,
		Configuration,
		Play
	};

	// ----- Serverbound Packet IDs -----------------------------------

	namespace Serverbound {

		namespace Status {
			constexpr int32_t StatusRequest = 0x00;
			constexpr int32_t PingRequest   = 0x01;
		}

		namespace Login {
			constexpr int32_t Hello              = 0x00;
			constexpr int32_t Key                = 0x01;
			constexpr int32_t CustomQueryAnswer   = 0x02;
			constexpr int32_t LoginAcknowledged   = 0x03;
			constexpr int32_t CookieResponse      = 0x04;
		}

		namespace Config {
			constexpr int32_t ClientInformation    = 0x00;
			constexpr int32_t CookieResponse       = 0x01;
			constexpr int32_t CustomPayload        = 0x02;
			constexpr int32_t FinishConfiguration  = 0x03;
			constexpr int32_t KeepAlive            = 0x04;
			constexpr int32_t Pong                 = 0x05;
			constexpr int32_t ResourcePack         = 0x06;
			constexpr int32_t SelectKnownPacks     = 0x07;
			constexpr int32_t CustomClickAction    = 0x08;
			constexpr int32_t AcceptCodeOfConduct  = 0x09;
		}

		namespace Play {
			constexpr int32_t AcceptTeleportation     = 0x00;
			constexpr int32_t Attack                  = 0x01;
			constexpr int32_t BlockEntityTagQuery      = 0x02;
			constexpr int32_t BundleItemSelected       = 0x03;
			constexpr int32_t ChangeDifficulty         = 0x04;
			constexpr int32_t ChangeGameMode           = 0x05;
			constexpr int32_t ChatAck                  = 0x06;
			constexpr int32_t ChatCommand              = 0x07;
			constexpr int32_t ChatCommandSigned        = 0x08;
			constexpr int32_t Chat                     = 0x09;
			constexpr int32_t ChatSessionUpdate        = 0x0A;
			constexpr int32_t ChunkBatchReceived       = 0x0B;
			constexpr int32_t ClientCommand            = 0x0C;
			constexpr int32_t ClientTickEnd            = 0x0D;
			constexpr int32_t ClientInformation        = 0x0E;
			constexpr int32_t CommandSuggestion        = 0x0F;
			constexpr int32_t ConfigurationAcknowledged = 0x10;
			constexpr int32_t ContainerButtonClick     = 0x11;
			constexpr int32_t ContainerClick           = 0x12;
			constexpr int32_t ContainerClose           = 0x13;
			constexpr int32_t ContainerSlotStateChanged = 0x14;
			constexpr int32_t CookieResponse           = 0x15;
			constexpr int32_t CustomPayload            = 0x16;
			constexpr int32_t DebugSubscriptionRequest = 0x17;
			constexpr int32_t EditBook                 = 0x18;
			constexpr int32_t EntityTagQuery           = 0x19;
			constexpr int32_t Interact                 = 0x1A;
			constexpr int32_t JigsawGenerate           = 0x1B;
			constexpr int32_t KeepAlive                = 0x1C;
			constexpr int32_t LockDifficulty           = 0x1D;
			constexpr int32_t MovePlayerPosition       = 0x1E;
			constexpr int32_t MovePlayerPositionRotation = 0x1F;
			constexpr int32_t MovePlayerRotation       = 0x20;
			constexpr int32_t MovePlayerStatusOnly     = 0x21;
			constexpr int32_t MoveVehicle              = 0x22;
			constexpr int32_t PaddleBoat               = 0x23;
			constexpr int32_t PickItemFromBlock        = 0x24;
			constexpr int32_t PickItemFromEntity       = 0x25;
			constexpr int32_t PingRequest              = 0x26;
			constexpr int32_t PlaceRecipe              = 0x27;
			constexpr int32_t PlayerAbilities          = 0x28;
			constexpr int32_t PlayerAction             = 0x29;
			constexpr int32_t PlayerCommand            = 0x2A;
			constexpr int32_t PlayerInput              = 0x2B;
			constexpr int32_t PlayerLoaded             = 0x2C;
			constexpr int32_t Pong                     = 0x2D;
			constexpr int32_t RecipeBookChangeSettings = 0x2E;
			constexpr int32_t RecipeBookSeenRecipe     = 0x2F;
			constexpr int32_t RenameItem               = 0x30;
			constexpr int32_t ResourcePack             = 0x31;
			constexpr int32_t SeenAdvancements         = 0x32;
			constexpr int32_t SelectTrade              = 0x33;
			constexpr int32_t SetBeacon                = 0x34;
			constexpr int32_t SetCarriedItem           = 0x35;
			constexpr int32_t SetCommandBlock          = 0x36;
			constexpr int32_t SetCommandMinecart       = 0x37;
			constexpr int32_t SetCreativeModeSlot      = 0x38;
			constexpr int32_t SetGameRule              = 0x39;
			constexpr int32_t SetJigsawBlock           = 0x3A;
			constexpr int32_t SetStructureBlock        = 0x3B;
			constexpr int32_t SetTestBlock             = 0x3C;
			constexpr int32_t SignUpdate               = 0x3D;
			constexpr int32_t SpectateEntity           = 0x3E;
			constexpr int32_t Swing                    = 0x3F;
			constexpr int32_t TeleportToEntity         = 0x40;
			constexpr int32_t TestInstanceBlockAction   = 0x41;
			constexpr int32_t UseItemOn                = 0x42;
			constexpr int32_t UseItem                  = 0x43;
			constexpr int32_t CustomClickAction        = 0x44;
		}
	}

	// ----- Clientbound Packet IDs -----------------------------------

	namespace Clientbound {

		namespace Status {
			constexpr int32_t StatusResponse = 0x00;
			constexpr int32_t PongResponse   = 0x01;
		}

		namespace Login {
			constexpr int32_t Disconnect       = 0x00;
			constexpr int32_t Hello            = 0x01;
			constexpr int32_t LoginFinished    = 0x02;
			constexpr int32_t LoginCompression = 0x03;
			constexpr int32_t CustomQuery      = 0x04;
			constexpr int32_t CookieRequest    = 0x05;
		}

		namespace Config {
			constexpr int32_t CookieRequest         = 0x00;
			constexpr int32_t CustomPayload         = 0x01;
			constexpr int32_t Disconnect            = 0x02;
			constexpr int32_t FinishConfiguration   = 0x03;
			constexpr int32_t KeepAlive             = 0x04;
			constexpr int32_t Ping                  = 0x05;
			constexpr int32_t ResetChat             = 0x06;
			constexpr int32_t RegistryData          = 0x07;
			constexpr int32_t ResourcePackPop       = 0x08;
			constexpr int32_t ResourcePackPush      = 0x09;
			constexpr int32_t StoreCookie           = 0x0A;
			constexpr int32_t Transfer              = 0x0B;
			constexpr int32_t UpdateEnabledFeatures = 0x0C;
			constexpr int32_t UpdateTags            = 0x0D;
			constexpr int32_t SelectKnownPacks      = 0x0E;
			constexpr int32_t CustomReportDetails   = 0x0F;
			constexpr int32_t ServerLinks           = 0x10;
			constexpr int32_t ClearDialog           = 0x11;
			constexpr int32_t ShowDialog            = 0x12;
			constexpr int32_t CodeOfConduct         = 0x13;
		}

		namespace Play {
			constexpr int32_t BundleDelimiter        = 0x00;
			constexpr int32_t AddEntity              = 0x01;
			constexpr int32_t Animate                = 0x02;
			constexpr int32_t AwardStats             = 0x03;
			constexpr int32_t BlockChangedAck        = 0x04;
			constexpr int32_t BlockDestruction       = 0x05;
			constexpr int32_t BlockEntityData        = 0x06;
			constexpr int32_t BlockEvent             = 0x07;
			constexpr int32_t BlockUpdate            = 0x08;
			constexpr int32_t BossEvent              = 0x09;
			constexpr int32_t ChangeDifficulty       = 0x0A;
			constexpr int32_t ChunkBatchFinished     = 0x0B;
			constexpr int32_t ChunkBatchStart        = 0x0C;
			constexpr int32_t ChunksBiomes           = 0x0D;
			constexpr int32_t ClearTitles            = 0x0E;
			constexpr int32_t CommandSuggestions      = 0x0F;
			constexpr int32_t Commands               = 0x10;
			constexpr int32_t ContainerClose         = 0x11;
			constexpr int32_t ContainerSetContent    = 0x12;
			constexpr int32_t ContainerSetData       = 0x13;
			constexpr int32_t ContainerSetSlot       = 0x14;
			constexpr int32_t CookieRequest          = 0x15;
			constexpr int32_t Cooldown               = 0x16;
			constexpr int32_t CustomChatCompletions  = 0x17;
			constexpr int32_t CustomPayload          = 0x18;
			constexpr int32_t DamageEvent            = 0x19;
			constexpr int32_t DebugBlockValue        = 0x1A;
			constexpr int32_t DebugChunkValue        = 0x1B;
			constexpr int32_t DebugEntityValue       = 0x1C;
			constexpr int32_t DebugEvent             = 0x1D;
			constexpr int32_t DebugSample            = 0x1E;
			constexpr int32_t DeleteChat             = 0x1F;
			constexpr int32_t Disconnect             = 0x20;
			constexpr int32_t DisguisedChat          = 0x21;
			constexpr int32_t EntityEvent            = 0x22;
			constexpr int32_t EntityPositionSync     = 0x23;
			constexpr int32_t Explode                = 0x24;
			constexpr int32_t ForgetLevelChunk       = 0x25;
			constexpr int32_t GameEvent              = 0x26;
			constexpr int32_t GameRuleValues         = 0x27;
			constexpr int32_t GameTestHighlightPos   = 0x28;
			constexpr int32_t MountScreenOpen        = 0x29;
			constexpr int32_t HurtAnimation          = 0x2A;
			constexpr int32_t InitializeBorder       = 0x2B;
			constexpr int32_t KeepAlive              = 0x2C;
			constexpr int32_t LevelChunkWithLight    = 0x2D;
			constexpr int32_t LevelEvent             = 0x2E;
			constexpr int32_t LevelParticles         = 0x2F;
			constexpr int32_t LightUpdate            = 0x30;
			constexpr int32_t Login                  = 0x31;
			constexpr int32_t LowDiskSpaceWarning    = 0x32;
			constexpr int32_t MapItemData            = 0x33;
			constexpr int32_t MerchantOffers         = 0x34;
			constexpr int32_t MoveEntityPosition     = 0x35;
			constexpr int32_t MoveEntityPositionRotation = 0x36;
			constexpr int32_t MoveMinecartAlongTrack = 0x37;
			constexpr int32_t MoveEntityRotation     = 0x38;
			constexpr int32_t MoveVehicle            = 0x39;
			constexpr int32_t OpenBook               = 0x3A;
			constexpr int32_t OpenScreen             = 0x3B;
			constexpr int32_t OpenSignEditor         = 0x3C;
			constexpr int32_t Ping                   = 0x3D;
			constexpr int32_t PongResponse           = 0x3E;
			constexpr int32_t PlaceGhostRecipe       = 0x3F;
			constexpr int32_t PlayerAbilities        = 0x40;
			constexpr int32_t PlayerChat             = 0x41;
			constexpr int32_t PlayerCombatEnd        = 0x42;
			constexpr int32_t PlayerCombatEnter      = 0x43;
			constexpr int32_t PlayerCombatKill       = 0x44;
			constexpr int32_t PlayerInfoRemove       = 0x45;
			constexpr int32_t PlayerInfoUpdate       = 0x46;
			constexpr int32_t PlayerLookAt           = 0x47;
			constexpr int32_t PlayerPosition         = 0x48;
			constexpr int32_t PlayerRotation         = 0x49;
			constexpr int32_t RecipeBookAdd          = 0x4A;
			constexpr int32_t RecipeBookRemove       = 0x4B;
			constexpr int32_t RecipeBookSettings     = 0x4C;
			constexpr int32_t RemoveEntities         = 0x4D;
			constexpr int32_t RemoveMobEffect        = 0x4E;
			constexpr int32_t ResetScore             = 0x4F;
			constexpr int32_t ResourcePackPop        = 0x50;
			constexpr int32_t ResourcePackPush       = 0x51;
			constexpr int32_t Respawn                = 0x52;
			constexpr int32_t RotateHead             = 0x53;
			constexpr int32_t SectionBlocksUpdate    = 0x54;
			constexpr int32_t SelectAdvancementsTab  = 0x55;
			constexpr int32_t ServerData             = 0x56;
			constexpr int32_t SetActionBarText       = 0x57;
			constexpr int32_t SetBorderCenter        = 0x58;
			constexpr int32_t SetBorderLerpSize      = 0x59;
			constexpr int32_t SetBorderSize          = 0x5A;
			constexpr int32_t SetBorderWarningDelay  = 0x5B;
			constexpr int32_t SetBorderWarningDistance = 0x5C;
			constexpr int32_t SetCamera              = 0x5D;
			constexpr int32_t SetChunkCacheCenter    = 0x5E;
			constexpr int32_t SetChunkCacheRadius    = 0x5F;
			constexpr int32_t SetCursorItem          = 0x60;
			constexpr int32_t SetDefaultSpawnPosition = 0x61;
			constexpr int32_t SetDisplayObjective    = 0x62;
			constexpr int32_t SetEntityData          = 0x63;
			constexpr int32_t SetEntityLink          = 0x64;
			constexpr int32_t SetEntityMotion        = 0x65;
			constexpr int32_t SetEquipment           = 0x66;
			constexpr int32_t SetExperience          = 0x67;
			constexpr int32_t SetHealth              = 0x68;
			constexpr int32_t SetHeldSlot            = 0x69;
			constexpr int32_t SetObjective           = 0x6A;
			constexpr int32_t SetPassengers          = 0x6B;
			constexpr int32_t SetPlayerInventory     = 0x6C;
			constexpr int32_t SetPlayerTeam          = 0x6D;
			constexpr int32_t SetScore               = 0x6E;
			constexpr int32_t SetSimulationDistance   = 0x6F;
			constexpr int32_t SetSubtitleText        = 0x70;
			constexpr int32_t SetTime                = 0x71;
			constexpr int32_t SetTitleText           = 0x72;
			constexpr int32_t SetTitlesAnimation     = 0x73;
			constexpr int32_t SoundEntity            = 0x74;
			constexpr int32_t Sound                  = 0x75;
			constexpr int32_t StartConfiguration     = 0x76;
			constexpr int32_t StopSound              = 0x77;
			constexpr int32_t StoreCookie            = 0x78;
			constexpr int32_t SystemChat             = 0x79;
			constexpr int32_t TabList                = 0x7A;
			constexpr int32_t TagQuery               = 0x7B;
			constexpr int32_t TakeItemEntity         = 0x7C;
			constexpr int32_t TeleportEntity         = 0x7D;
			constexpr int32_t TestInstanceBlockStatus = 0x7E;
			constexpr int32_t TickingState           = 0x7F;
			constexpr int32_t TickingStep            = 0x80;
			constexpr int32_t Transfer               = 0x81;
			constexpr int32_t UpdateAdvancements     = 0x82;
			constexpr int32_t UpdateAttributes       = 0x83;
			constexpr int32_t UpdateMobEffect        = 0x84;
			constexpr int32_t UpdateRecipes          = 0x85;
			constexpr int32_t UpdateTags             = 0x86;
			constexpr int32_t ProjectilePower        = 0x87;
			constexpr int32_t CustomReportDetails    = 0x88;
			constexpr int32_t ServerLinks            = 0x89;
			constexpr int32_t Waypoint               = 0x8A;
			constexpr int32_t ClearDialog            = 0x8B;
			constexpr int32_t ShowDialog             = 0x8C;
		}
	}

}
