; ModuleID = 'top'

%CN6SystemC6stringV = type { void (%CN6SystemC6string)* }
%CN6SystemC6string = type { %CN6SystemC6stringV*, i32 }
%CN6SystemC2IOV = type { void (%CN6SystemC2IO)* }
%CN6SystemC2IO = type { %CN6SystemC2IOV* }

@_VCN6SystemC6string = external global %CN6SystemC6stringV
@_VCN6SystemC2IO = global %CN6SystemC2IOV zeroinitializer

declare i8* @malloc(i64)

declare void @_CCN6SystemC6string(%CN6SystemC6string*)

define void @_CCN6SystemC2IO(%CN6SystemC2IO*) {
  %2 = getelementptr inbounds %CN6SystemC2IO, %CN6SystemC2IO* %0, i32 0, i32 0
  store %CN6SystemC2IOV* @_VCN6SystemC2IO, %CN6SystemC2IOV** %2
  ret void
}

declare i32 @printf(i8*, ...)
@writeIntFmt = private constant [3 x i8] c"%d\00"
@writelnFmt = private constant [4 x i8] c"%s\0A\00"

define void @CN6SystemC2IOF8writeInt1i(i32 %a) {
"CN6SystemC2IOF8writeInt1i@entry":
  call void @llvm.dbg.declare(metadata i32 %a, metadata !15, metadata !16), !dbg !17
  %0 = getelementptr [3 x i8], [3 x i8]* @writeIntFmt, i32 0, i32 0
  call i32 (i8*, ...) @printf(i8* %0, i32 %a)
  ret void
}

; Function Attrs: nounwind readnone
declare void @llvm.dbg.declare(metadata, metadata, metadata) #0

define void @CN6SystemC2IOF7writeln1a(%CN6SystemC6string* %s) {
"CN6SystemC2IOF7writeln1a@entry":
  call void @llvm.dbg.declare(metadata %CN6SystemC6string* %s, metadata !18, metadata !16), !dbg !19
  %0 = getelementptr [4 x i8], [4 x i8]* @writelnFmt, i32 0, i32 0
  %1 = getelementptr %CN6SystemC6string, %CN6SystemC6string* %s, i32 0, i32 1
  %2 = load i32, i32* %1
  %3 = inttoptr i32 %2 to i8*
  call i32 (i8*, ...) @printf(i8* %0, i8* %3)
  ret void
}

attributes #0 = { nounwind readnone }

!llvm.module.flags = !{!0, !1}
!llvm.dbg.cu = !{!2}

!0 = !{i32 2, !"Dwarf Version", i32 4}
!1 = !{i32 2, !"Debug Info Version", i32 3}
!2 = distinct !DICompileUnit(language: DW_LANG_C, file: !3, producer: "jcc 0.0.1", isOptimized: false, runtimeVersion: 1, emissionKind: 1, enums: !4, subprograms: !5)
!3 = !DIFile(filename: "lib/io.jas", directory: "/home/rododo/jascal")
!4 = !{}
!5 = !{!6, !11}
!6 = !DISubprogram(name: "writeInt", linkageName: "CN6SystemC2IOF8writeInt1i", scope: !7, file: !7, line: 7, type: !8, isLocal: false, isDefinition: true, scopeLine: 7, isOptimized: false, function: void (i32)* @CN6SystemC2IOF8writeInt1i, variables: !4)
!7 = !DIFile(filename: "io.jas", directory: "lib/")
!8 = !DISubroutineType(types: !9)
!9 = !{null, !10}
!10 = !DIBasicType(name: "int", size: 32, align: 32, encoding: DW_ATE_signed)
!11 = !DISubprogram(name: "writeln", linkageName: "CN6SystemC2IOF7writeln1a", scope: !7, file: !7, line: 11, type: !12, isLocal: false, isDefinition: true, scopeLine: 11, isOptimized: false, function: void (%CN6SystemC6string*)* @CN6SystemC2IOF7writeln1a, variables: !4)
!12 = !DISubroutineType(types: !13)
!13 = !{null, !14}
!14 = !DIBasicType(name: "string", size: 64, align: 64, encoding: DW_ATE_float)
!15 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "a", arg: 0, scope: !6, file: !7, line: 7, type: !10)
!16 = !DIExpression()
!17 = !DILocation(line: 7, column: 40, scope: !6)
!18 = !DILocalVariable(tag: DW_TAG_arg_variable, name: "s", arg: 0, scope: !11, file: !7, line: 11, type: !14)
!19 = !DILocation(line: 11, column: 42, scope: !11)
