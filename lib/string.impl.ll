; ModuleID = 'top'
target triple = "i386-unknown-linux-gnu"

%CN6SystemC6stringV = type { void (%CN6SystemC6string)* }
%CN6SystemC6string = type { %CN6SystemC6stringV*, i32 }

@_VCN6SystemC6string = global %CN6SystemC6stringV zeroinitializer

declare i8* @malloc(i64)

define void @_CCN6SystemC6string(%CN6SystemC6string*) {
  %2 = getelementptr inbounds %CN6SystemC6string, %CN6SystemC6string* %0, i32 0, i32 0
  store %CN6SystemC6stringV* @_VCN6SystemC6string, %CN6SystemC6stringV** %2
  %3 = getelementptr inbounds %CN6SystemC6string, %CN6SystemC6string* %0, i32 0, i32 1
  store i32 0, i32* %3
  ret void
}

!llvm.module.flags = !{!0, !1}
!llvm.dbg.cu = !{!2}

!0 = !{i32 2, !"Dwarf Version", i32 4}
!1 = !{i32 2, !"Debug Info Version", i32 3}
!2 = distinct !DICompileUnit(language: DW_LANG_C, file: !3, producer: "jcc 0.0.1", isOptimized: false, runtimeVersion: 1, emissionKind: 1, enums: !4)
!3 = !DIFile(filename: "lib/string.jas", directory: "/home/rododo/jascal")
!4 = !{}
