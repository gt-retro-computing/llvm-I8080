// RUN: %clang_cc1 -triple x86_64-unknown-unknown -fopenmp -ast-dump %s | FileCheck --match-full-lines -implicit-check-not=openmp_structured_block %s

void test() {
#pragma omp critical
  ;
}

// CHECK: TranslationUnitDecl {{.*}} <<invalid sloc>> <invalid sloc>
// CHECK: `-FunctionDecl {{.*}} <{{.*}}ast-dump-openmp-critical.c:3:1, line:6:1> line:3:6 test 'void ()'
// CHECK-NEXT:   `-CompoundStmt {{.*}} <col:13, line:6:1>
// CHECK-NEXT:     `-OMPCriticalDirective {{.*}} <line:4:1, col:21>
// CHECK-NEXT:       `-CapturedStmt {{.*}} <line:5:3>
// CHECK-NEXT:         `-CapturedDecl {{.*}} <<invalid sloc>> <invalid sloc>
// CHECK-NEXT:           |-NullStmt {{.*}} <col:3> openmp_structured_block
// CHECK-NEXT:           `-ImplicitParamDecl {{.*}} <line:4:1> col:1 implicit __context 'struct (anonymous at {{.*}}ast-dump-openmp-critical.c:4:1) *const restrict'
