; RUN: opt -S %loadintelquantumpasses -p=q-annotations-to-json,spin-native-annotate,print-circuit-qbb --print-format tex --quantum-debug=PrintLatex -o /dev/null %s 2>&1 | FileCheck %s --dump-input fail
; ModuleID = 'test.ll'
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%"class.std::ios_base::Init" = type { i8 }
%"struct.iqsdk::IqsConfig" = type { %"struct.iqsdk::DeviceConfig.base", i32, %"class.std::__cxx11::basic_string", double, i8, i64 }
%"struct.iqsdk::DeviceConfig.base" = type <{ ptr, %"class.std::__cxx11::basic_string", i8, i8 }>
%"class.std::__cxx11::basic_string" = type { %"struct.std::__cxx11::basic_string<char>::_Alloc_hider", i64, %union.anon }
%"struct.std::__cxx11::basic_string<char>::_Alloc_hider" = type { ptr }
%union.anon = type { i64, [8 x i8] }
%"class.iqsdk::FullStateSimulator" = type { %"class.iqsdk::SimulatorDevice" }
%"class.iqsdk::SimulatorDevice" = type { %"class.iqsdk::Device" }
%"class.iqsdk::Device" = type { ptr, i8, %"class.std::shared_ptr" }
%"class.std::shared_ptr" = type { %"class.std::__shared_ptr" }
%"class.std::__shared_ptr" = type { ptr, %"class.std::__shared_count" }
%"class.std::__shared_count" = type { ptr }
%"struct.iqsdk::DeviceConfig" = type <{ ptr, %"class.std::__cxx11::basic_string", i8, i8, [6 x i8] }>

$_ZN5iqsdk9IqsConfigD2Ev = comdat any

@.str = private unnamed_addr constant [325 x i8] c"{     \22matrix_real\22 : \221/sqrt(2)*[1, 1, 1, -1]\22,     \22matrix_imag\22 : [0, 0, 0, 0],     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : true,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [4],     \22identifier\22 : 3   }\00", section "llvm.metadata"
@.str.1 = private unnamed_addr constant [37 x i8] c"include/clang/Quantum/quintrinsics.h\00", section "llvm.metadata"
@.str.2 = private unnamed_addr constant [313 x i8] c"{     \22matrix_real\22 : [0, 1, 1, 0],     \22matrix_imag\22 : [0, 0, 0, 0],     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : true,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [1],     \22identifier\22 : 12   }\00", section "llvm.metadata"
@.str.3 = private unnamed_addr constant [314 x i8] c"{     \22matrix_real\22 : [0, 0, 0, 0],     \22matrix_imag\22 : [0, -1, 1, 0],     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : true,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [2],     \22identifier\22 : 18   }\00", section "llvm.metadata"
@.str.4 = private unnamed_addr constant [313 x i8] c"{     \22matrix_real\22 : [1, 0, 0, -1],     \22matrix_imag\22 : [0, 0, 0, 0],     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : true,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [3],     \22identifier\22 : 6   }\00", section "llvm.metadata"
@.str.5 = private unnamed_addr constant [313 x i8] c"{     \22matrix_real\22 : [1, 0, 0, 0],     \22matrix_imag\22 : [0, 0, 0, 1],     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [3],     \22identifier\22 : 5   }\00", section "llvm.metadata"
@.str.6 = private unnamed_addr constant [315 x i8] c"{     \22matrix_real\22 : [1, 0, 0, 0],     \22matrix_imag\22 : [0, 0, 0, -1],     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [3],     \22identifier\22 : 11   }\00", section "llvm.metadata"
@.str.7 = private unnamed_addr constant [334 x i8] c"{     \22matrix_real\22 : \22[1, 0, 0, cos(pi/4)]\22,     \22matrix_imag\22 : \22[0, 0, 0, sin(pi/4)]\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [3],     \22identifier\22 : 24   }\00", section "llvm.metadata"
@.str.8 = private unnamed_addr constant [335 x i8] c"{     \22matrix_real\22 : \22[1, 0, 0, cos(pi/4)]\22,     \22matrix_imag\22 : \22[0, 0, 0, -sin(pi/4)]\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [3],     \22identifier\22 : 25   }\00", section "llvm.metadata"
@.str.9 = private unnamed_addr constant [364 x i8] c"{     \22matrix_real\22 : \22[cos(angle/2), 0, 0, cos(angle/2)]\22,     \22matrix_imag\22 : \22[0, -sin(angle/2), sin(angle/2), 0]\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [1],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [1],     \22identifier\22 : 26   }\00", section "llvm.metadata"
@.str.10 = private unnamed_addr constant [362 x i8] c"{     \22matrix_real\22 : \22[cos(angle/2), -sin(angle/2), sin(angle/2), cos(angle/2)]\22,     \22matrix_imag\22 : [0, 0, 0, 0],     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [1],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [2],     \22identifier\22 : 27   }\00", section "llvm.metadata"
@.str.11 = private unnamed_addr constant [364 x i8] c"{     \22matrix_real\22 : \22[cos(angle/2), 0, 0, cos(angle/2)]\22,     \22matrix_imag\22 : \22[-sin(angle/2), 0, 0, sin(angle/2)]\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [1],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [3],     \22identifier\22 : 28   }\00", section "llvm.metadata"
@.str.12 = private unnamed_addr constant [396 x i8] c"{     \22matrix_real\22 : [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, -1],     \22matrix_imag\22 : [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : true,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0, 1],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [0, 1],     \22local_basis_list\22 : [3, 3],     \22identifier\22 : 37   }\00", section "llvm.metadata"
@.str.13 = private unnamed_addr constant [392 x i8] c"{     \22matrix_real\22 : [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0],     \22matrix_imag\22 : [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : true,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0, 1],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [0],     \22local_basis_list\22 : [3, 1],     \22identifier\22 : 35   }\00", section "llvm.metadata"
@.str.14 = private unnamed_addr constant [391 x i8] c"{     \22matrix_real\22 : [1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1],     \22matrix_imag\22 : [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : true,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0, 1],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [0, 0],     \22identifier\22 : 38   }\00", section "llvm.metadata"
@.str.15 = private unnamed_addr constant [689 x i8] c"{     \22matrix_real\22 : [1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0],     \22matrix_imag\22 : [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : true,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0, 1, 2],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [0, 1],     \22local_basis_list\22 : [3, 3, 1],     \22identifier\22 : 39   }\00", section "llvm.metadata"
@.str.16 = private unnamed_addr constant [417 x i8] c"{     \22matrix_real\22 : \22{qt(prob0) : [0.5, 0.5, 0.5, 0.5], qt(prob1) : [0.5, -0.5, 0.5, -0.5]}\22,     \22matrix_imag\22 : \22{qt(prob0) : [0, 0, 0, 0], qt(prob1) : [0, 0, 0, 0]}\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : false,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [1],     \22identifier\22 : 40   }\00", section "llvm.metadata"
@.str.17 = private unnamed_addr constant [417 x i8] c"{     \22matrix_real\22 : \22{qt(prob0) : [0.5, 0, 0, 0.5], qt(prob1) : [0.5, 0, 0, -0.5]}\22,     \22matrix_imag\22 : \22{qt(prob0) : [0, 0.5, -0.5, 0], qt(prob1) : [0, 0.5, 0.5, 0]}\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : false,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [2],     \22identifier\22 : 41   }\00", section "llvm.metadata"
@.str.18 = private unnamed_addr constant [399 x i8] c"{     \22matrix_real\22 : \22{qt(prob0) : [1, 0, 0, 0], qt(prob1) : [0, 1, 0, 0]}\22,     \22matrix_imag\22 : \22{qt(prob0) : [0, 0, 0, 0], qt(prob1) : [0, 0, 0, 0]}\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : false,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [3],     \22identifier\22 : 42   }\00", section "llvm.metadata"
@.str.19 = private unnamed_addr constant [421 x i8] c"{     \22matrix_real\22 : \22{qt(prob0) : [0.5, 0.5, 0.5, 0.5], qt(prob1) : [0.5, -0.5, -0.5, 0.5]}\22,     \22matrix_imag\22 : \22{qt(prob0) : [0, 0, 0, 0], qt(prob1) : [0, 0, 0, 0]}\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : false,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [1, 2],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [1],     \22identifier\22 : 43   }\00", section "llvm.metadata"
@.str.20 = private unnamed_addr constant [421 x i8] c"{     \22matrix_real\22 : \22{qt(prob0) : [0.5, 0, 0, 0.5], qt(prob1) : [0.5, 0, 0, 0.5]}\22,     \22matrix_imag\22 : \22{qt(prob0) : [0, 0.5, -0.5, 0], qt(prob1) : [0, -0.5, 0.5, 0]}\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : false,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [1, 2],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [2],     \22identifier\22 : 44   }\00", section "llvm.metadata"
@.str.21 = private unnamed_addr constant [403 x i8] c"{     \22matrix_real\22 : \22{qt(prob0) : [1, 0, 0, 0], qt(prob1) : [0, 0, 0, 1]}\22,     \22matrix_imag\22 : \22{qt(prob0) : [0, 0, 0, 0], qt(prob1) : [0, 0, 0, 0]}\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : false,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [1, 2],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [3],     \22identifier\22 : 45   }\00", section "llvm.metadata"
@.str.22 = private unnamed_addr constant [420 x i8] c"{     \22matrix_real\22 : \22[1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, cos(angle)]\22,     \22matrix_imag\22 : \22[0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -sin(angle)]\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0, 1],     \22parametric_list\22 : [2],     \22control_qubit_list\22 : [0, 1],     \22local_basis_list\22 : [3, 3],     \22identifier\22 : 46   }\00", section "llvm.metadata"
@.str.23 = private unnamed_addr constant [423 x i8] c"{     \22matrix_real\22 : \22[cos(theta/2), -sin(theta/2) sin(phi), sin(theta/2) sin(phi), cos(theta/2)]\22,     \22matrix_imag\22 : \22[0, -sin(theta/2)cos(phi), sin(theta/2)cos(phi), 0]\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0],     \22parametric_list\22 : [1,2],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [0],     \22identifier\22 : 47   }\00", section "llvm.metadata"
@.str.24 = private unnamed_addr constant [503 x i8] c"{     \22matrix_real\22 : \22[1, 0, 0, 0, 0, (1+cos(angle))/2, (1-cos(angle))/2, 0, 0, (1-cos(angle))/2, (1+cos(angle))/2, 0, 0, 0, 0, 1]\22,     \22matrix_imag\22 : \22[1, 0, 0, 0, 0, sin(angle)/2, -sin(angle)/2, 0, 0, -sin(angle)/2, sin(angle)/2, 0, 0, 0, 0, 1]\22,     \22matrix_order\22 : \22rm\22,     \22is_hermitian\22 : false,     \22is_unitary\22 : true,     \22is_mutable\22 : true,     \22qubit_list\22 : [0, 1],     \22parametric_list\22 : [2],     \22control_qubit_list\22 : [],     \22local_basis_list\22 : [0, 0],     \22identifier\22 : 53   }\00", section "llvm.metadata"
@_ZStL8__ioinit = internal global %"class.std::ios_base::Init" zeroinitializer, align 1
@__dso_handle = external hidden global i8
@q = dso_local global [4 x i16] zeroinitializer, align 2
@c = dso_local local_unnamed_addr global [4 x i8] zeroinitializer, align 1
@.str.25 = private unnamed_addr constant [10 x i8] c"noiseless\00", align 1
@_ZTVN5iqsdk9IqsConfigE = external unnamed_addr constant { [5 x ptr] }, align 8
@_ZTVN5iqsdk12DeviceConfigE = external unnamed_addr constant { [5 x ptr] }, align 8
@llvm.global_ctors = appending global [1 x { i32, ptr, ptr }] [{ i32, ptr, ptr } { i32 65535, ptr @_GLOBAL__sub_I_bell_states.cpp, ptr null }]
@llvm.global.annotations = appending global [24 x { ptr, ptr, ptr, i32, ptr }] [{ ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str, ptr @.str.1, i32 48, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.2, ptr @.str.1, i32 68, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.3, ptr @.str.1, i32 88, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.4, ptr @.str.1, i32 109, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.5, ptr @.str.1, i32 129, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.6, ptr @.str.1, i32 149, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.7, ptr @.str.1, i32 169, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.8, ptr @.str.1, i32 189, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.9, ptr @.str.1, i32 209, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.10, ptr @.str.1, i32 230, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.11, ptr @.str.1, i32 251, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.12, ptr @.str.1, i32 272, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.13, ptr @.str.1, i32 293, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.14, ptr @.str.1, i32 314, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.15, ptr @.str.1, i32 335, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.16, ptr @.str.1, i32 359, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.17, ptr @.str.1, i32 377, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.18, ptr @.str.1, i32 395, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.19, ptr @.str.1, i32 413, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.20, ptr @.str.1, i32 433, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.21, ptr @.str.1, i32 453, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.22, ptr @.str.1, i32 481, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.23, ptr @.str.1, i32 503, ptr null }, { ptr, ptr, ptr, i32, ptr } { ptr undef, ptr @.str.24, ptr @.str.1, i32 525, ptr null }], section "llvm.metadata"

declare void @_ZNSt8ios_base4InitC1Ev(ptr noundef nonnull align 1 dereferenceable(1)) unnamed_addr #0

; Function Attrs: nounwind
declare void @_ZNSt8ios_base4InitD1Ev(ptr noundef nonnull align 1 dereferenceable(1)) unnamed_addr #1

; Function Attrs: nofree nounwind
declare i32 @__cxa_atexit(ptr, ptr, ptr) local_unnamed_addr #2

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z8BellPrepv.stub() local_unnamed_addr #3 section ".qbbs_text" {
entry:
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %arrayidx15 = getelementptr inbounds [4 x i16], ptr @q, i64 0, i64 0
  %0 = load i16, ptr %arrayidx15, align 2
  call void @llvm.quantum.rotxy(i16 %0, double 0x3FF921FB5443D6F4, double 0x3FF921FB54442D18, i32 1)
  %arrayidx14 = getelementptr inbounds [4 x i16], ptr @q, i64 0, i64 1
  %1 = load i16, ptr %arrayidx14, align 2
  call void @llvm.quantum.rotxy(i16 %1, double 0x3FF921FB5443D6F4, double 0x3FF921FB54442D18, i32 0)
  %arrayidx13 = getelementptr inbounds [4 x i16], ptr @q, i64 0, i64 2
  %2 = load i16, ptr %arrayidx13, align 2
  call void @llvm.quantum.rotxy(i16 %2, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx11 = getelementptr inbounds [4 x i16], ptr @q, i64 0, i64 0
  %3 = load i16, ptr %arrayidx11, align 2
  %arrayidx12 = getelementptr inbounds [4 x i16], ptr @q, i64 0, i64 2
  %4 = load i16, ptr %arrayidx12, align 2
  call void @llvm.quantum.cphase(i16 %3, i16 %4, double 0x400921FB54442D18, i32 0)
  %arrayidx10 = getelementptr inbounds [4 x i16], ptr @q, i64 0, i64 2
  %5 = load i16, ptr %arrayidx10, align 2
  call void @llvm.quantum.rotxy(i16 %5, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx9 = getelementptr inbounds [4 x i16], ptr @q, i64 0, i64 3
  %6 = load i16, ptr %arrayidx9, align 2
  call void @llvm.quantum.rotxy(i16 %6, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx7 = getelementptr inbounds [4 x i16], ptr @q, i64 0, i64 1
  %7 = load i16, ptr %arrayidx7, align 2
  %arrayidx8 = getelementptr inbounds [4 x i16], ptr @q, i64 0, i64 3
  %8 = load i16, ptr %arrayidx8, align 2
  call void @llvm.quantum.cphase(i16 %7, i16 %8, double 0x400921FB54442D18, i32 0)
  %arrayidx6 = getelementptr inbounds [4 x i16], ptr @q, i64 0, i64 3
  %9 = load i16, ptr %arrayidx6, align 2
  call void @llvm.quantum.rotxy(i16 %9, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 2)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  ret void
}

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(i64 immarg, ptr nocapture) #4

; Function Attrs: nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.end.p0(i64 immarg, ptr nocapture) #4

; Function Attrs: mustprogress noinline nounwind uwtable
define dso_local void @_Z17state_preparationv.stub() local_unnamed_addr #3 section ".qbbs_text" {
entry:
  br label %aqcc.quantum

aqcc.quantum:                                     ; preds = %entry
  %arrayidx23 = getelementptr inbounds [4 x i16], ptr @q, i64 0, i64 0
  %0 = load i16, ptr %arrayidx23, align 2
  call void @llvm.quantum.prepz(i16 %0, i32 1)
  %arrayidx22 = getelementptr inbounds [4 x i16], ptr @q, i64 0, i64 1
  %1 = load i16, ptr %arrayidx22, align 2
  call void @llvm.quantum.prepz(i16 %1, i32 0)
  %arrayidx21 = getelementptr inbounds [4 x i16], ptr @q, i64 0, i64 2
  %2 = load i16, ptr %arrayidx21, align 2
  call void @llvm.quantum.prepz(i16 %2, i32 0)
  %arrayidx20 = getelementptr inbounds [4 x i16], ptr @q, i64 0, i64 3
  %3 = load i16, ptr %arrayidx20, align 2
  call void @llvm.quantum.prepz(i16 %3, i32 0)
  %arrayidx19 = getelementptr inbounds [4 x i16], ptr @q, i64 0, i64 0
  %4 = load i16, ptr %arrayidx19, align 2
  call void @llvm.quantum.rotxy(i16 %4, double 0x3FF921FB5443D6F4, double 0x3FF921FB54442D18, i32 0)
  %arrayidx18 = getelementptr inbounds [4 x i16], ptr @q, i64 0, i64 1
  %5 = load i16, ptr %arrayidx18, align 2
  call void @llvm.quantum.rotxy(i16 %5, double 0x3FF921FB5443D6F4, double 0x3FF921FB54442D18, i32 0)
  %arrayidx17 = getelementptr inbounds [4 x i16], ptr @q, i64 0, i64 2
  %6 = load i16, ptr %arrayidx17, align 2
  call void @llvm.quantum.rotxy(i16 %6, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx15 = getelementptr inbounds [4 x i16], ptr @q, i64 0, i64 0
  %7 = load i16, ptr %arrayidx15, align 2
  %arrayidx16 = getelementptr inbounds [4 x i16], ptr @q, i64 0, i64 2
  %8 = load i16, ptr %arrayidx16, align 2
  call void @llvm.quantum.cphase(i16 %7, i16 %8, double 0x400921FB54442D18, i32 0)
  %arrayidx14 = getelementptr inbounds [4 x i16], ptr @q, i64 0, i64 2
  %9 = load i16, ptr %arrayidx14, align 2
  call void @llvm.quantum.rotxy(i16 %9, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 0)
  %arrayidx13 = getelementptr inbounds [4 x i16], ptr @q, i64 0, i64 3
  %10 = load i16, ptr %arrayidx13, align 2
  call void @llvm.quantum.rotxy(i16 %10, double 0x3FF921FB54442D18, double 0x4012D97C7F3321D2, i32 0)
  %arrayidx11 = getelementptr inbounds [4 x i16], ptr @q, i64 0, i64 1
  %11 = load i16, ptr %arrayidx11, align 2
  %arrayidx12 = getelementptr inbounds [4 x i16], ptr @q, i64 0, i64 3
  %12 = load i16, ptr %arrayidx12, align 2
  call void @llvm.quantum.cphase(i16 %11, i16 %12, double 0x400921FB54442D18, i32 0)
  %arrayidx10 = getelementptr inbounds [4 x i16], ptr @q, i64 0, i64 3
  %13 = load i16, ptr %arrayidx10, align 2
  call void @llvm.quantum.rotxy(i16 %13, double 0x3FF921FB54442D18, double 0x3FF921FB54442D18, i32 2)
  br label %aqcc.meas.move.end

aqcc.meas.move.end:                               ; preds = %aqcc.quantum
  ret void
}

; Function Attrs: norecurse uwtable
define dso_local noundef i32 @main() local_unnamed_addr #5 personality ptr @__gxx_personality_v0 {
entry:
  %iqs_config = alloca %"struct.iqsdk::IqsConfig", align 8
  %agg.tmp = alloca %"class.std::__cxx11::basic_string", align 8
  %iqs_device = alloca %"class.iqsdk::FullStateSimulator", align 8
  call void @llvm.lifetime.start.p0(i64 noundef 104, ptr noundef nonnull align 8 dereferenceable(104) %iqs_config) #10
  %0 = getelementptr inbounds %"class.std::__cxx11::basic_string", ptr %agg.tmp, i64 0, i32 2
  store ptr %0, ptr %agg.tmp, align 8, !tbaa !0
  call void @llvm.memcpy.p0.p0.i64(ptr noundef nonnull writeonly align 8 dereferenceable(9) %0, ptr noundef nonnull readonly align 1 dereferenceable(9) @.str.25, i64 noundef 9, i1 noundef false)
  %_M_string_length.i.i.i.i.i.i = getelementptr inbounds %"class.std::__cxx11::basic_string", ptr %agg.tmp, i64 0, i32 1
  store i64 9, ptr %_M_string_length.i.i.i.i.i.i, align 8, !tbaa !5
  %arrayidx.i.i.i.i.i = getelementptr inbounds %"class.std::__cxx11::basic_string", ptr %agg.tmp, i64 0, i32 2, i32 1, i64 1
  store i8 0, ptr %arrayidx.i.i.i.i.i, align 1, !tbaa !8
  %call = call i64 @time(ptr noundef null) #10
  invoke void @_ZN5iqsdk9IqsConfigC1EiNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEbmbd(ptr noundef nonnull align 8 dereferenceable(104) %iqs_config, i32 noundef 4, ptr noundef nonnull %agg.tmp, i1 noundef zeroext false, i64 noundef %call, i1 noundef zeroext true, double noundef 1.000000e-02)
          to label %invoke.cont2 unwind label %lpad1

invoke.cont2:                                     ; preds = %entry
  %1 = load ptr, ptr %agg.tmp, align 8, !tbaa !9
  %cmp.i.i.i = icmp eq ptr %1, %0
  br i1 %cmp.i.i.i, label %_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev.exit, label %if.then.i.i

if.then.i.i:                                      ; preds = %invoke.cont2
  call void @_ZdlPv(ptr noundef %1) #10
  br label %_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev.exit

_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev.exit: ; preds = %if.then.i.i, %invoke.cont2
  call void @llvm.lifetime.start.p0(i64 noundef 32, ptr noundef nonnull %iqs_device) #10
  invoke void @_ZN5iqsdk18FullStateSimulatorC1ERNS_12DeviceConfigE(ptr noundef nonnull align 8 dereferenceable(32) %iqs_device, ptr noundef nonnull align 8 dereferenceable(104) %iqs_config)
          to label %invoke.cont5 unwind label %lpad4

invoke.cont5:                                     ; preds = %_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev.exit
  %call8 = invoke noundef i32 @_ZN5iqsdk6Device5readyEv(ptr noundef nonnull align 8 dereferenceable(32) %iqs_device)
          to label %invoke.cont7 unwind label %lpad6

invoke.cont7:                                     ; preds = %invoke.cont5
  %cmp.not = icmp eq i32 %call8, 0
  br i1 %cmp.not, label %if.end, label %cleanup

lpad1:                                            ; preds = %entry
  %2 = landingpad { ptr, i32 }
          cleanup
  %3 = load ptr, ptr %agg.tmp, align 8, !tbaa !9
  %cmp.i.i.i23 = icmp eq ptr %3, %0
  br i1 %cmp.i.i.i23, label %ehcleanup16, label %if.then.i.i24

if.then.i.i24:                                    ; preds = %lpad1
  call void @_ZdlPv(ptr noundef %3) #10
  br label %ehcleanup16

lpad4:                                            ; preds = %_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev.exit
  %4 = landingpad { ptr, i32 }
          cleanup
  br label %ehcleanup12

lpad6:                                            ; preds = %invoke.cont5
  %5 = landingpad { ptr, i32 }
          cleanup
  call void @_ZN5iqsdk6DeviceD2Ev(ptr noundef nonnull align 8 dereferenceable(32) %iqs_device) #10
  br label %ehcleanup12

if.end:                                           ; preds = %invoke.cont7
  call void @_Z17state_preparationv.stub() #10
  br label %cleanup

cleanup:                                          ; preds = %if.end, %invoke.cont7
  %retval.0 = phi i32 [ 1, %invoke.cont7 ], [ 0, %if.end ]
  call void @_ZN5iqsdk6DeviceD2Ev(ptr noundef nonnull align 8 dereferenceable(32) %iqs_device) #10
  call void @llvm.lifetime.end.p0(i64 noundef 32, ptr noundef nonnull %iqs_device) #10
  store ptr getelementptr inbounds ({ [5 x ptr] }, ptr @_ZTVN5iqsdk9IqsConfigE, i64 0, inrange i32 0, i64 2), ptr %iqs_config, align 8, !tbaa !10
  %simulation_type.i = getelementptr inbounds %"struct.iqsdk::IqsConfig", ptr %iqs_config, i64 0, i32 2
  %6 = load ptr, ptr %simulation_type.i, align 8, !tbaa !9
  %7 = getelementptr inbounds %"struct.iqsdk::IqsConfig", ptr %iqs_config, i64 0, i32 2, i32 2
  %cmp.i.i.i.i = icmp eq ptr %6, %7
  br i1 %cmp.i.i.i.i, label %_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev.exit.i, label %if.then.i.i.i

if.then.i.i.i:                                    ; preds = %cleanup
  call void @_ZdlPv(ptr noundef %6) #10
  br label %_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev.exit.i

_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev.exit.i: ; preds = %if.then.i.i.i, %cleanup
  store ptr getelementptr inbounds ({ [5 x ptr] }, ptr @_ZTVN5iqsdk12DeviceConfigE, i64 0, inrange i32 0, i64 2), ptr %iqs_config, align 8, !tbaa !10
  %backend.i.i = getelementptr inbounds %"struct.iqsdk::DeviceConfig", ptr %iqs_config, i64 0, i32 1
  %8 = load ptr, ptr %backend.i.i, align 8, !tbaa !9
  %9 = getelementptr inbounds %"struct.iqsdk::DeviceConfig", ptr %iqs_config, i64 0, i32 1, i32 2
  %cmp.i.i.i.i.i = icmp eq ptr %8, %9
  br i1 %cmp.i.i.i.i.i, label %_ZN5iqsdk9IqsConfigD2Ev.exit, label %if.then.i.i.i.i

if.then.i.i.i.i:                                  ; preds = %_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev.exit.i
  call void @_ZdlPv(ptr noundef %8) #10
  br label %_ZN5iqsdk9IqsConfigD2Ev.exit

_ZN5iqsdk9IqsConfigD2Ev.exit:                     ; preds = %if.then.i.i.i.i, %_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev.exit.i
  call void @llvm.lifetime.end.p0(i64 noundef 104, ptr noundef nonnull %iqs_config) #10
  ret i32 %retval.0

ehcleanup12:                                      ; preds = %lpad6, %lpad4
  %.pn19 = phi { ptr, i32 } [ %5, %lpad6 ], [ %4, %lpad4 ]
  call void @llvm.lifetime.end.p0(i64 noundef 32, ptr noundef nonnull %iqs_device) #10
  call void @_ZN5iqsdk9IqsConfigD2Ev(ptr noundef nonnull align 8 dereferenceable(104) %iqs_config) #10
  br label %ehcleanup16

ehcleanup16:                                      ; preds = %ehcleanup12, %if.then.i.i24, %lpad1
  %.pn19.pn = phi { ptr, i32 } [ %.pn19, %ehcleanup12 ], [ %2, %lpad1 ], [ %2, %if.then.i.i24 ]
  call void @llvm.lifetime.end.p0(i64 noundef 104, ptr noundef nonnull %iqs_config) #10
  resume { ptr, i32 } %.pn19.pn
}

declare i32 @__gxx_personality_v0(...)

; Function Attrs: nounwind
declare i64 @time(ptr noundef) local_unnamed_addr #1

declare void @_ZN5iqsdk9IqsConfigC1EiNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEEbmbd(ptr noundef nonnull align 8 dereferenceable(104), i32 noundef, ptr noundef, i1 noundef zeroext, i64 noundef, i1 noundef zeroext, double noundef) unnamed_addr #0

declare void @_ZN5iqsdk18FullStateSimulatorC1ERNS_12DeviceConfigE(ptr noundef nonnull align 8 dereferenceable(32), ptr noundef nonnull align 8 dereferenceable(42)) unnamed_addr #0

declare noundef i32 @_ZN5iqsdk6Device5readyEv(ptr noundef nonnull align 8 dereferenceable(32)) local_unnamed_addr #0

; Function Attrs: nounwind
declare void @_ZN5iqsdk6DeviceD2Ev(ptr noundef nonnull align 8 dereferenceable(32)) unnamed_addr #1

; Function Attrs: inlinehint nounwind uwtable
define linkonce_odr dso_local void @_ZN5iqsdk9IqsConfigD2Ev(ptr noundef nonnull align 8 dereferenceable(104) %this) unnamed_addr #6 comdat align 2 personality ptr @__gxx_personality_v0 {
entry:
  store ptr getelementptr inbounds ({ [5 x ptr] }, ptr @_ZTVN5iqsdk9IqsConfigE, i64 0, inrange i32 0, i64 2), ptr %this, align 8, !tbaa !10
  %simulation_type = getelementptr inbounds %"struct.iqsdk::IqsConfig", ptr %this, i64 0, i32 2
  %0 = load ptr, ptr %simulation_type, align 8, !tbaa !9
  %1 = getelementptr inbounds %"struct.iqsdk::IqsConfig", ptr %this, i64 0, i32 2, i32 2
  %cmp.i.i.i = icmp eq ptr %0, %1
  br i1 %cmp.i.i.i, label %_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev.exit, label %if.then.i.i

if.then.i.i:                                      ; preds = %entry
  tail call void @_ZdlPv(ptr noundef %0) #10
  br label %_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev.exit

_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev.exit: ; preds = %if.then.i.i, %entry
  store ptr getelementptr inbounds ({ [5 x ptr] }, ptr @_ZTVN5iqsdk12DeviceConfigE, i64 0, inrange i32 0, i64 2), ptr %this, align 8, !tbaa !10
  %backend.i = getelementptr inbounds %"struct.iqsdk::DeviceConfig", ptr %this, i64 0, i32 1
  %2 = load ptr, ptr %backend.i, align 8, !tbaa !9
  %3 = getelementptr inbounds %"struct.iqsdk::DeviceConfig", ptr %this, i64 0, i32 1, i32 2
  %cmp.i.i.i.i = icmp eq ptr %2, %3
  br i1 %cmp.i.i.i.i, label %_ZN5iqsdk12DeviceConfigD2Ev.exit, label %if.then.i.i.i

if.then.i.i.i:                                    ; preds = %_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev.exit
  tail call void @_ZdlPv(ptr noundef %2) #10
  br label %_ZN5iqsdk12DeviceConfigD2Ev.exit

_ZN5iqsdk12DeviceConfigD2Ev.exit:                 ; preds = %if.then.i.i.i, %_ZNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEED2Ev.exit
  ret void
}

; Function Attrs: nobuiltin nounwind
declare void @_ZdlPv(ptr noundef) local_unnamed_addr #7

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i64(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i64, i1 immarg) #8

; Function Attrs: uwtable
define internal void @_GLOBAL__sub_I_bell_states.cpp() #9 section ".text.startup" {
entry:
  tail call void @_ZNSt8ios_base4InitC1Ev(ptr noundef nonnull align 1 dereferenceable(1) @_ZStL8__ioinit)
  %0 = tail call i32 @__cxa_atexit(ptr noundef nonnull @_ZNSt8ios_base4InitD1Ev, ptr noundef nonnull @_ZStL8__ioinit, ptr noundef nonnull @__dso_handle) #10
  ret void
}

declare i32 @_ZN4qrt021initializeQrtSystem64EPdPcS1_(ptr, ptr, ptr)

declare i1 @_ZN4qrt013isInitializedEv()

declare void @_ZN4qrt020updateMeasurementMapEPbmb(ptr, i64, i1)

declare i32 @_ZN4qrt014scheduleKernelEPFvvEj(ptr, i32)

declare i32 @_ZN4qrt018dismantleQrtSystemEv()

; Function Attrs: nounwind
declare void @llvm.quantum.rotz(i16, double, i32 immarg) #10

; Function Attrs: nounwind
declare void @llvm.quantum.rotxy(i16, double, double, i32 immarg) #10

; Function Attrs: nounwind
declare void @llvm.quantum.cphase(i16, i16, double, i32 immarg) #10

; Function Attrs: nounwind
declare void @llvm.quantum.swapalp(i16, i16, double, i32 immarg) #10

; Function Attrs: nounwind
declare void @llvm.quantum.measz(i16, ptr, i1, i32 immarg) #10

; Function Attrs: nounwind
declare void @llvm.quantum.prepz(i16, i32 immarg) #10

; Function Attrs: nounwind
declare void @llvm.quantum.rotz.imm(i16 immarg, double immarg, i32 immarg) #10

; Function Attrs: nounwind
declare void @llvm.quantum.rotxy.imm(i16 immarg, double immarg, double immarg, i32 immarg) #10

; Function Attrs: nounwind
declare void @llvm.quantum.cphase.imm(i16 immarg, i16 immarg, double immarg, i32 immarg) #10

; Function Attrs: nounwind
declare void @llvm.quantum.swapalp.imm(i16 immarg, i16 immarg, double immarg, i32 immarg) #10

; Function Attrs: nounwind
declare void @llvm.quantum.measz.imm(i16 immarg, i32 immarg) #10

; Function Attrs: nounwind
declare void @llvm.quantum.prepz.imm(i16 immarg, i32 immarg) #10

; Function Attrs: nounwind
declare void @llvm.quantum.rotz.imm.index(i16 immarg, i32 immarg, i32 immarg) #10

; Function Attrs: nounwind
declare void @llvm.quantum.rotxy.imm.index(i16 immarg, i32 immarg, i32 immarg, i32 immarg) #10

; Function Attrs: nounwind
declare void @llvm.quantum.cphase.imm.index(i16 immarg, i16 immarg, i32 immarg, i32 immarg) #10

; Function Attrs: nounwind
declare void @llvm.quantum.swapalp.imm.index(i16 immarg, i16 immarg, i32 immarg, i32 immarg) #10

; Function Attrs: nounwind
declare void @llvm.quantum.measz.imm.index(i16 immarg, i32 immarg, i32 immarg) #10

attributes #0 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nounwind "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { nofree nounwind }
attributes #3 = { mustprogress noinline nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }
attributes #5 = { norecurse uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #6 = { inlinehint nounwind uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #7 = { nobuiltin nounwind "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #8 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }
attributes #9 = { uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #10 = { nounwind }

!0 = !{!1, !2, i64 0}
!1 = !{!"_ZTSNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEE12_Alloc_hiderE", !2, i64 0}
!2 = !{!"any pointer", !3, i64 0}
!3 = !{!"omnipotent char", !4, i64 0}
!4 = !{!"Simple C++ TBAA"}
!5 = !{!6, !7, i64 8}
!6 = !{!"_ZTSNSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE", !1, i64 0, !7, i64 8, !3, i64 16}
!7 = !{!"long", !3, i64 0}
!8 = !{!3, !3, i64 0}
!9 = !{!6, !2, i64 0}
!10 = !{!11, !11, i64 0}
!11 = !{!"vtable pointer", !4, i64 0}
; CHECK: LaTex instructions: 
; CHECK: \lstick{\ket{q\_0}} & \gate{RXY(1.571, 1.571)} & \ctrl{2} & \qw & \qw & \qw \\ 
; CHECK: \lstick{\ket{q\_1}} & \gate{RXY(1.571, 1.571)} & \qw & \ctrl{2} & \qw & \qw \\ 
; CHECK: \lstick{\ket{q\_2}} & \gate{RXY(1.571, 4.712)} & \gate{Z} & \qw & \gate{RXY(1.571, 1.571)} & \qw \\ 
; CHECK: \lstick{\ket{q\_3}} & \gate{RXY(1.571, 4.712)} & \qw & \gate{Z} & \gate{RXY(1.571, 1.571)} & \qw 
; CHECK: LaTex instructions: 
; CHECK: \lstick{\ket{q\_0}} & \gate{PrepZ} & \gate{RXY(1.571, 1.571)} & \ctrl{2} & \qw & \qw & \qw \\ 
; CHECK: \lstick{\ket{q\_1}} & \gate{PrepZ} & \gate{RXY(1.571, 1.571)} & \qw & \ctrl{2} & \qw & \qw \\ 
; CHECK: \lstick{\ket{q\_2}} & \gate{PrepZ} & \gate{RXY(1.571, 4.712)} & \gate{Z} & \qw & \gate{RXY(1.571, 1.571)} & \qw \\ 
; CHECK: \lstick{\ket{q\_3}} & \gate{PrepZ} & \gate{RXY(1.571, 4.712)} & \qw & \gate{Z} & \gate{RXY(1.571, 1.571)} & \qw 
