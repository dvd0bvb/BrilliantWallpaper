/**
 *
 *  @file      TestTomlConfigBuilder.cpp
 *  @author    David Brill
 *  @copyright © David Brill, 2024. All right reserved.
 *
 *  Unit tests for the TomlConfigBuilder class
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fstream>

#include "TomlConfigBuilder.hpp"

TEST(TestTomlConfigBuilder, testBuildGoodfileIstream) {
  std::ifstream file("files/goodfile.toml");
  EXPECT_TRUE(file);

  brilliant::wp::TomlConfigBuilder builder;
  std::optional<brilliant::wp::Config> config;
  EXPECT_NO_THROW(config.emplace(builder.build(file)));

  EXPECT_EQ(config->globalTransitionDelay, std::chrono::minutes{10});
  EXPECT_EQ(config->monitors.size(), 2);
  EXPECT_EQ(config->monitors[2].backgroundPaths.size(), 1);
  EXPECT_EQ(config->monitors[2].backgroundPaths[0].string(), "testfile.png");
  EXPECT_TRUE(config->monitors[2].index.has_value());
  EXPECT_EQ(config->monitors[2].index.value(), 2);
  EXPECT_TRUE(config->monitors[2].transitionDelay.has_value());
  EXPECT_EQ(config->monitors[2].transitionDelay.value(),
            std::chrono::minutes{20});
  EXPECT_EQ(config->monitors[0].backgroundPaths.size(), 2);
  EXPECT_EQ(config->monitors[0].backgroundPaths[0].string(),
            "anothertestfile.jpg");
  EXPECT_EQ(config->monitors[0].backgroundPaths[1].string(), "dir/onemorefile.bmp");
  EXPECT_FALSE(config->monitors[0].index.has_value());
  EXPECT_FALSE(config->monitors[0].transitionDelay.has_value());
}

TEST(TestTomlConfigBuilder, testBuildGoodFilePath) {
  brilliant::wp::TomlConfigBuilder builder;
  std::optional<brilliant::wp::Config> config;
  EXPECT_NO_THROW(config.emplace(builder.build("files/goodfile.toml")));

  EXPECT_EQ(config->globalTransitionDelay, std::chrono::minutes{10});
  EXPECT_EQ(config->monitors.size(), 2);
  EXPECT_EQ(config->monitors[2].backgroundPaths.size(), 1);
  EXPECT_EQ(config->monitors[2].backgroundPaths[0].string(), "testfile.png");
  EXPECT_TRUE(config->monitors[2].index.has_value());
  EXPECT_EQ(config->monitors[2].index.value(), 2);
  EXPECT_TRUE(config->monitors[2].transitionDelay.has_value());
  EXPECT_EQ(config->monitors[2].transitionDelay.value(),
            std::chrono::minutes{20});
  EXPECT_EQ(config->monitors[0].backgroundPaths.size(), 2);
  EXPECT_EQ(config->monitors[0].backgroundPaths[0].string(),
            "anothertestfile.jpg");
  EXPECT_EQ(config->monitors[0].backgroundPaths[1].string(), "dir/onemorefile.bmp");
  EXPECT_FALSE(config->monitors[0].index.has_value());
  EXPECT_FALSE(config->monitors[0].transitionDelay.has_value());
}

TEST(TestTomlConfigBuilder, testBuildNottoml) {
  brilliant::wp::TomlConfigBuilder builder;

  { 
    std::ifstream file("files/nottoml.toml");
    EXPECT_THROW(builder.build(file), brilliant::wp::ConfigError);
  }

  EXPECT_THROW(builder.build("files/nottoml.toml"), brilliant::wp::ConfigError);
}