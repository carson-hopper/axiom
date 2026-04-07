#include "TestFramework.h"

#include "Axiom/Environment/Level/Chunk.h"

using namespace Axiom;

TEST(ChunkTest, InitialState) {
	Chunk chunk(10, 20);

	ASSERT_EQ(chunk.ChunkX(), 10);
	ASSERT_EQ(chunk.ChunkZ(), 20);
	ASSERT_EQ(chunk.AllocatedSectionCount(), 0);
	ASSERT_EQ(chunk.BiomeId(), 0);
}

TEST(ChunkTest, GetBlockReturnsAirForEmpty) {
	Chunk chunk(0, 0);

	// Any position should return 0 (air) for unallocated sections
	ASSERT_EQ(chunk.GetBlockState(0, 0, 0), 0);
	ASSERT_EQ(chunk.GetBlockState(8, 100, 8), 0);
	ASSERT_EQ(chunk.GetBlockState(15, 300, 15), 0);
}

TEST(ChunkTest, SetBlockAllocatesSection) {
	Chunk chunk(0, 0);

	ASSERT_EQ(chunk.AllocatedSectionCount(), 0);

	// Place a block
	chunk.SetBlockState(5, 70, 5, 42);

	ASSERT_EQ(chunk.AllocatedSectionCount(), 1);
	ASSERT_EQ(chunk.GetBlockState(5, 70, 5), 42);
}

TEST(ChunkTest, SetAirDoesNotAllocate) {
	Chunk chunk(0, 0);

	chunk.SetBlockState(5, 70, 5, 0); // 0 = air

	ASSERT_EQ(chunk.AllocatedSectionCount(), 0);
}

TEST(ChunkTest, BlockPositionMapping) {
	Chunk chunk(0, 0);

	// Set blocks at different positions
	chunk.SetBlockState(0, 0, 0, 1);
	chunk.SetBlockState(15, 15, 15, 2);
	chunk.SetBlockState(7, 100, 7, 3);

	ASSERT_EQ(chunk.GetBlockState(0, 0, 0), 1);
	ASSERT_EQ(chunk.GetBlockState(15, 15, 15), 2);
	ASSERT_EQ(chunk.GetBlockState(7, 100, 7), 3);
}

TEST(ChunkTest, OutOfBoundsY) {
	Chunk chunk(0, 0);

	// Below minimum Y
	chunk.SetBlockState(0, -100, 0, 42);
	ASSERT_EQ(chunk.GetBlockState(0, -100, 0), 0);

	// Above maximum Y
	chunk.SetBlockState(0, 500, 0, 42);
	ASSERT_EQ(chunk.GetBlockState(0, 500, 0), 0);
}

TEST(ChunkTest, BiomeId) {
	Chunk chunk(0, 0);

	chunk.SetBiomeId(5);
	ASSERT_EQ(chunk.BiomeId(), 5);
}

TEST(ChunkTest, SectionAllocation) {
	Chunk chunk(0, 0);

	// Blocks at same Y level should use same section
	chunk.SetBlockState(0, 70, 0, 1);
	chunk.SetBlockState(15, 79, 15, 2);

	ASSERT_EQ(chunk.AllocatedSectionCount(), 1);

	// Block at different Y level (different section)
	chunk.SetBlockState(0, 100, 0, 3);

	ASSERT_EQ(chunk.AllocatedSectionCount(), 2);
}

TEST(ChunkTest, MemoryUsage) {
	Chunk chunk(0, 0);

	size_t initialUsage = chunk.MemoryUsage();
	ASSERT_EQ(initialUsage, sizeof(Chunk));

	chunk.SetBlockState(0, 70, 0, 1);

	size_t afterAlloc = chunk.MemoryUsage();
	ASSERT_TRUE(afterAlloc > initialUsage);
}