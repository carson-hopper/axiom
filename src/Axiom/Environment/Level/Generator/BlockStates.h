#pragma once

#include <cstdint>

namespace Axiom {

	/**
	 * Block state IDs extracted from MC 26.1.
	 * These are the default state IDs for each block type.
	 */
	namespace BlockState {
		constexpr int32_t Air = 0;
		constexpr int32_t Stone = 1;
		constexpr int32_t GrassBlock = 9;
		constexpr int32_t Dirt = 10;
		constexpr int32_t CoarseDirt = 11;
		constexpr int32_t Podzol = 13;
		constexpr int32_t Bedrock = 85;
		constexpr int32_t Water = 86;
		constexpr int32_t Lava = 102;
		constexpr int32_t Sand = 118;
		constexpr int32_t RedSand = 123;
		constexpr int32_t Gravel = 124;
		constexpr int32_t GoldOre = 129;
		constexpr int32_t DeepslateGoldOre = 130;
		constexpr int32_t IronOre = 131;
		constexpr int32_t DeepslateIronOre = 132;
		constexpr int32_t CoalOre = 133;
		constexpr int32_t DeepslateCoalOre = 134;
		constexpr int32_t OakLog = 137;
		constexpr int32_t SpruceLog = 140;
		constexpr int32_t BirchLog = 143;
		constexpr int32_t OakLeaves = 279;
		constexpr int32_t SpruceLeaves = 307;
		constexpr int32_t BirchLeaves = 335;
		constexpr int32_t LapisOre = 563;
		constexpr int32_t DeepslateLapisOre = 564;
		constexpr int32_t Sandstone = 578;
		constexpr int32_t ShortGrass = 2248;
		constexpr int32_t Fern = 2249;
		constexpr int32_t Dandelion = 2321;
		constexpr int32_t Poppy = 2324;
		constexpr int32_t DiamondOre = 5307;
		constexpr int32_t DeepslateDiamondOre = 5308;
		constexpr int32_t RedstoneOre = 6882;
		constexpr int32_t DeepslateRedstoneOre = 6884;
		constexpr int32_t Snow = 6919;
		constexpr int32_t Cactus = 6929;
		constexpr int32_t Terracotta = 12912;
		constexpr int32_t PackedIce = 12914;
		constexpr int32_t CopperOre = 25313;
		constexpr int32_t DeepslateCopperOre = 25314;
		constexpr int32_t Deepslate = 27924;
		constexpr int32_t Mud = 27922;
	}

}
