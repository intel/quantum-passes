//===- QuantumModule.cpp -------------------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file implements a super-class which contains all of the meta information
// regarding a quantum compilation.
//
//===----------------------------------------------------------------------===//

#include "IntelQuantumPasses/QuantumAnalysis/QuantumModule.h"
#include "IntelQuantumPasses/QuantumUtils/QuantumGeneralUtils.h"
#include "llvm/ADT/DenseSet.h"
#include "llvm/AsmParser/Parser.h"
#include "llvm/Demangle/Demangle.h"
#include "llvm/Support/raw_ostream.h"

#include <cmath> // std::ceil
#include <fstream>
#include <optional>

namespace llvm {
namespace aqcc {

std::shared_ptr<QuantumModule> QuantumModuleProxy::QM;

std::string getParentDirectory(const std::string &full_path) {
  char separator = '/';
#ifdef _WIN32
  separator = '\\';
#endif
  const size_t last_position = full_path.rfind(separator);
  if (last_position != std::string::npos) {
    return (full_path.substr(0, last_position));
  }
  return full_path;
}

QuantumModule::QuantumModule(const std::string &filename,
                             const std::string &json_filename)
    : sdk_json_filename(json_filename) {
  std::unique_ptr<Module> mod_ptr = parseAssemblyFile(filename, Err, C);
  base = mod_ptr.release();
  is_base_owned = true;
  initializeData(*base);
}

std::string truncateName(std::string &FullName) {
  size_t SpaceLoc = FullName.find(" ");
  size_t ParenLoc = FullName.find("(");
  ;
  if (SpaceLoc > ParenLoc || SpaceLoc == std::string::npos) {
    SpaceLoc = 0;
  } else {
    SpaceLoc += 1;
  }
  return FullName.substr(SpaceLoc, ParenLoc - SpaceLoc);
}

void QuantumModule::initializeData(Module &M) {

  // populate quantum kernels

  DenseSet<Function *> FunctionAsKernels;
  for (QuantumKernel &QK : q_kernel_list) {
    FunctionAsKernels.insert(QK.getFunction());
  }

  for (auto &F : M) {
    if (q_gate_metadata.find(F.getName().str()) != q_gate_metadata.end()) {
      std::string FullName = llvm::demangle(F.getName().str());
      std::string TruncName = truncateName(FullName);
      HumanNameToFunction.insert(std::make_pair(TruncName, &F));
    }
    if (FunctionAsKernels.find(&F) == FunctionAsKernels.end()) {
      if (isQKernel(F) || isQBBSText(F)) {
        q_kernel_list.push_back(QuantumKernel(F));
        std::string FullName = llvm::demangle(F.getName().str());
        std::string TruncName = truncateName(FullName);
        HumanNameToFunction.insert(std::make_pair(TruncName, &F));
      }

      // Because FLEQ can turn any function into a QK (if it has a fixing call)
      // we have to check if the functions has QBBs if post FLEQ compilation
      else if (has_fleq) {
        QuantumKernel temp(F, true);
        if (temp.begin() != temp.end()) {
          q_kernel_list.push_back(temp);
        }
      }
    }
  }

  // initialize the Platform

  platform = std::make_unique<TPL::PlatformConfiguration>();

  getPlatformConfigFilename();
  if (err_str_ == "")
    buildPlatformFromConfigFilename();

  // initialize qubit maps
  // TODO: if seg fault around QuantumRef use in the Analysis directory
  // is ever resolved, we can probably fold action of SimpleQbitPlacement
  // by uncomment the lines below.

  pos = QCompPosition::kBegin;
}

void QuantumModule::getPlatformConfigFilename() {

  if (sdk_json_filename.empty())
    return;

  std::ifstream json_file(sdk_json_filename);

  if (json_file.is_open()) {

    std::string file_contents((std::istreambuf_iterator<char>(json_file)),
                              std::istreambuf_iterator<char>());

    Expected<json::Value> E = json::parse(file_contents);

    if (auto err = E.takeError()) {
      err_str_ = "Unable to parse the contents of " + sdk_json_filename + ":\n";
      return;
    }

    if (json::Object *full_obj = E->getAsObject()) {
      if (json::Object *platform_obj = full_obj->getObject("platform")) {

        std::optional<StringRef> config_SR =
            platform_obj->getString("configuration");

        if (config_SR.has_value()) {
          platform_filename = config_SR.value().str();

          /// check if absolute path
          std::ifstream ifile_platform_abs(platform_filename);
          if (!ifile_platform_abs.is_open()) {
            /// check if relative path
            platform_filename =
                getParentDirectory(sdk_json_filename) + "/" + platform_filename;
            std::ifstream ifile_platform_rel(platform_filename);
            if (!ifile_platform_rel.is_open()) {
              err_str_ = "Unable to read the platform configuration file at " +
                         platform_filename + "\n";
            }
          }
        } else {
          err_str_ =
              "Unable to extract the platform configuration file name from " +
              sdk_json_filename + "\n";
          return;
        }
      } else {
        err_str_ = "Unable to extract the platform information from " +
                   sdk_json_filename + "\n";
        return;
      }
    } else {
      err_str_ = "Unable to extract the SDK JSON as a JSON Object from " +
                 sdk_json_filename + "\n";
      return;
    }
  } else {
    err_str_ = "Unable open the SDK JSON file at " + sdk_json_filename + "\n";
    return;
  }
}

void QuantumModule::buildPlatformFromConfigFilename() {

  if (platform_filename.empty())
    return;

  platform->loadPlatformConfig(platform_filename, false);
  has_platform = platform->platformConfigLoaded();
  if (has_platform) {
    machine.ClearAndResize();
    machine.CreateConnectivityGraphWithQubitProperties(*platform);
    machine.DetermineAvailableGatesAndDurations(*platform);
    machine.ComputeSwapDistanceMatrix();
    machine.ComputeDistanceMatrix();
  } else {
    err_str_ = "Couldn't load the platform configuration at " +
               platform_filename + "\n";
  }
}

int QuantumModule::getGateDurationInClockCycles(
    int id, const std::vector<unsigned> &qids) const {

  if (!has_platform)
    return -1;
  if (id < kNumGates)
    return -1;

  double duration_float =
      machine.GetDuration(getPlatformNameFromGateIdentifier(id), qids);

  if (duration_float > 0)
    return (int)std::ceil(duration_float / machine.GetUniqueClockCycleTime());
  else
    return -1;
}

int QuantumModule::getGateDurationInClockCycles(
    std::string gate_name, std::vector<unsigned> qids) const {

  if (!has_platform)
    return -1;

  double duration_float = machine.GetDuration(gate_name, qids);

  if (duration_float > 0)
    return (int)std::ceil(duration_float / machine.GetUniqueClockCycleTime());
  else
    return -1;
}

QuantumGates QuantumModule::gates() { return QuantumGates(*this); }

void QuantumModule::removeGate(GateDataMap::iterator it) {
  q_gate_metadata.erase(it);
}

QuantumKernel::QuantumKernel(Function &F, bool check) : qk_(&F) {
  is_a_qk_ = isQKernel(F);
  first_ = F.begin();

  if (is_a_qk_ || check)
    while (first_ != F.end()) {
      if (isQBasicBlock(*first_)) {
        is_a_qk_ = true;
        break;
      }
      ++first_;
    }
  else
    first_ = qk_->end();
}

QuantumKernel::iterator QuantumKernel::begin() {
  Function::iterator temp = qk_->begin();
  while (temp != qk_->end()) {
    if (isQBasicBlock(*temp)) {
      break;
    }
    ++temp;
  }
  return iterator(temp, qk_);
}

QuantumKernel::iterator QuantumKernel::begin() const {
  Function::iterator temp = qk_->begin();
  while (temp != qk_->end()) {
    if (isQBasicBlock(*temp)) {
      break;
    }
    ++temp;
  }
  return iterator(temp, qk_);
}

QuantumKernel::iterator QuantumKernel::iterator::operator++() {
  assert(it_ != F_->end());
  do {
    ++it_;
    if (it_ == F_->end())
      break;
  } while (!isQBasicBlock(*it_));
  return *this;
}

QuantumKernel::iterator QuantumKernel::iterator::operator--() {
  assert(it_ != F_->begin());
  do
    --it_;
  while (!isQBasicBlock(*it_) && it_ != F_->begin());
  // if the first BB is not a QBB, go to first QBB
  if (it_ == F_->begin() && !isQBasicBlock(*it_))
    return operator++();
  return *this;
}

QuantumKernel::scc_range::scc_range(Function *F) {
  for (scc_iterator<Function *> I = scc_begin(F), E = scc_end(F); I != E; ++I) {
    bool has_loop = I.hasCycle();
    std::vector<BasicBlock *> to_add;
    for (auto BB : *I) {
      if (isQBasicBlock(*BB))
        to_add.push_back(BB);
    }
    if (!to_add.empty())
      qbb_sccs_.push_back(std::make_pair(to_add, has_loop));
  }
}

} // namespace aqcc
} // namespace llvm
