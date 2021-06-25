; ModuleID = 'UnopEliminators.c'
source_filename = "UnopEliminators.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.UnopEliminatorList = type { %struct.UnopEliminator*, i32 }
%struct.UnopEliminator = type { %struct.Type*, %struct.Type*, void (%struct.Judgement*, %struct.Ast*)* }
%struct.Type = type { i32, %union.anon }
%union.anon = type { %struct.ProcType }
%struct.ProcType = type { %struct.Type*, %struct.Type* }
%struct.Judgement = type { i8, %union.anon.2 }
%union.anon.2 = type { %struct.Error }
%struct.Error = type { %struct.Location, i8* }
%struct.Location = type { i32, i32, i32, i32 }
%struct.Ast = type { i32, %union.anon.0 }
%union.anon.0 = type { %struct.Object }
%struct.Object = type { i32, %union.anon.1 }
%union.anon.1 = type { %struct.Lambda }
%struct.Lambda = type { %struct.Location, i64, %struct.Ast*, %struct.Ast*, %struct.SymbolTable*, i32, i8, i32 }
%struct.SymbolTable = type opaque

; Function Attrs: noinline nounwind optnone uwtable
define dso_local %struct.UnopEliminatorList* @CreateUnopEliminatorList() #0 {
  %1 = alloca %struct.UnopEliminatorList*, align 8
  %2 = call noalias i8* @malloc(i64 16) #2
  %3 = bitcast i8* %2 to %struct.UnopEliminatorList*
  store %struct.UnopEliminatorList* %3, %struct.UnopEliminatorList** %1, align 8
  %4 = load %struct.UnopEliminatorList*, %struct.UnopEliminatorList** %1, align 8
  %5 = getelementptr inbounds %struct.UnopEliminatorList, %struct.UnopEliminatorList* %4, i32 0, i32 0
  store %struct.UnopEliminator* null, %struct.UnopEliminator** %5, align 8
  %6 = load %struct.UnopEliminatorList*, %struct.UnopEliminatorList** %1, align 8
  %7 = getelementptr inbounds %struct.UnopEliminatorList, %struct.UnopEliminatorList* %6, i32 0, i32 1
  store i32 0, i32* %7, align 8
  %8 = load %struct.UnopEliminatorList*, %struct.UnopEliminatorList** %1, align 8
  ret %struct.UnopEliminatorList* %8
}

; Function Attrs: nounwind
declare dso_local noalias i8* @malloc(i64) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local void @DestroyUnopEliminatorList(%struct.UnopEliminatorList* %0) #0 {
  %2 = alloca %struct.UnopEliminatorList*, align 8
  store %struct.UnopEliminatorList* %0, %struct.UnopEliminatorList** %2, align 8
  %3 = load %struct.UnopEliminatorList*, %struct.UnopEliminatorList** %2, align 8
  %4 = getelementptr inbounds %struct.UnopEliminatorList, %struct.UnopEliminatorList* %3, i32 0, i32 0
  %5 = load %struct.UnopEliminator*, %struct.UnopEliminator** %4, align 8
  %6 = bitcast %struct.UnopEliminator* %5 to i8*
  call void @free(i8* %6) #2
  %7 = load %struct.UnopEliminatorList*, %struct.UnopEliminatorList** %2, align 8
  %8 = getelementptr inbounds %struct.UnopEliminatorList, %struct.UnopEliminatorList* %7, i32 0, i32 0
  store %struct.UnopEliminator* null, %struct.UnopEliminator** %8, align 8
  %9 = load %struct.UnopEliminatorList*, %struct.UnopEliminatorList** %2, align 8
  %10 = getelementptr inbounds %struct.UnopEliminatorList, %struct.UnopEliminatorList* %9, i32 0, i32 1
  store i32 0, i32* %10, align 8
  %11 = load %struct.UnopEliminatorList*, %struct.UnopEliminatorList** %2, align 8
  %12 = bitcast %struct.UnopEliminatorList* %11 to i8*
  call void @free(i8* %12) #2
  store %struct.UnopEliminatorList* null, %struct.UnopEliminatorList** %2, align 8
  ret void
}

; Function Attrs: nounwind
declare dso_local void @free(i8*) #1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local %struct.UnopEliminator* @InsertPrimitiveUnopEliminator(%struct.UnopEliminatorList* %0, %struct.Type* %1, %struct.Type* %2, void (%struct.Judgement*, %struct.Ast*)* %3) #0 {
  %5 = alloca %struct.UnopEliminator*, align 8
  %6 = alloca %struct.UnopEliminatorList*, align 8
  %7 = alloca %struct.Type*, align 8
  %8 = alloca %struct.Type*, align 8
  %9 = alloca void (%struct.Judgement*, %struct.Ast*)*, align 8
  %10 = alloca void (%struct.Judgement*, %struct.Ast*)*, align 8
  %11 = alloca %struct.UnopEliminator*, align 8
  store %struct.UnopEliminatorList* %0, %struct.UnopEliminatorList** %6, align 8
  store %struct.Type* %1, %struct.Type** %7, align 8
  store %struct.Type* %2, %struct.Type** %8, align 8
  store void (%struct.Judgement*, %struct.Ast*)* %3, void (%struct.Judgement*, %struct.Ast*)** %9, align 8
  store void (%struct.Judgement*, %struct.Ast*)* null, void (%struct.Judgement*, %struct.Ast*)** %10, align 8
  %12 = load %struct.UnopEliminatorList*, %struct.UnopEliminatorList** %6, align 8
  %13 = icmp ne %struct.UnopEliminatorList* %12, null
  br i1 %13, label %14, label %20

14:                                               ; preds = %4
  %15 = load %struct.Type*, %struct.Type** %7, align 8
  %16 = icmp ne %struct.Type* %15, null
  br i1 %16, label %17, label %20

17:                                               ; preds = %14
  %18 = load %struct.Type*, %struct.Type** %8, align 8
  %19 = icmp ne %struct.Type* %18, null
  br i1 %19, label %21, label %20

20:                                               ; preds = %17, %14, %4
  store %struct.UnopEliminator* null, %struct.UnopEliminator** %5, align 8
  br label %65

21:                                               ; preds = %17
  %22 = load %struct.UnopEliminatorList*, %struct.UnopEliminatorList** %6, align 8
  %23 = load %struct.Type*, %struct.Type** %7, align 8
  %24 = call %struct.UnopEliminator* @FindUnopEliminator(%struct.UnopEliminatorList* %22, %struct.Type* %23)
  store %struct.UnopEliminator* %24, %struct.UnopEliminator** %11, align 8
  %25 = load %struct.UnopEliminator*, %struct.UnopEliminator** %11, align 8
  %26 = icmp ne %struct.UnopEliminator* %25, null
  br i1 %26, label %63, label %27

27:                                               ; preds = %21
  %28 = load %struct.UnopEliminatorList*, %struct.UnopEliminatorList** %6, align 8
  %29 = getelementptr inbounds %struct.UnopEliminatorList, %struct.UnopEliminatorList* %28, i32 0, i32 1
  %30 = load i32, i32* %29, align 8
  %31 = add nsw i32 %30, 1
  store i32 %31, i32* %29, align 8
  %32 = load %struct.UnopEliminatorList*, %struct.UnopEliminatorList** %6, align 8
  %33 = getelementptr inbounds %struct.UnopEliminatorList, %struct.UnopEliminatorList* %32, i32 0, i32 0
  %34 = load %struct.UnopEliminator*, %struct.UnopEliminator** %33, align 8
  %35 = bitcast %struct.UnopEliminator* %34 to i8*
  %36 = load %struct.UnopEliminatorList*, %struct.UnopEliminatorList** %6, align 8
  %37 = getelementptr inbounds %struct.UnopEliminatorList, %struct.UnopEliminatorList* %36, i32 0, i32 1
  %38 = load i32, i32* %37, align 8
  %39 = sext i32 %38 to i64
  %40 = mul i64 24, %39
  %41 = call i8* @realloc(i8* %35, i64 %40) #2
  %42 = bitcast i8* %41 to %struct.UnopEliminator*
  %43 = load %struct.UnopEliminatorList*, %struct.UnopEliminatorList** %6, align 8
  %44 = getelementptr inbounds %struct.UnopEliminatorList, %struct.UnopEliminatorList* %43, i32 0, i32 0
  store %struct.UnopEliminator* %42, %struct.UnopEliminator** %44, align 8
  %45 = load %struct.UnopEliminatorList*, %struct.UnopEliminatorList** %6, align 8
  %46 = getelementptr inbounds %struct.UnopEliminatorList, %struct.UnopEliminatorList* %45, i32 0, i32 0
  %47 = load %struct.UnopEliminator*, %struct.UnopEliminator** %46, align 8
  %48 = load %struct.UnopEliminatorList*, %struct.UnopEliminatorList** %6, align 8
  %49 = getelementptr inbounds %struct.UnopEliminatorList, %struct.UnopEliminatorList* %48, i32 0, i32 1
  %50 = load i32, i32* %49, align 8
  %51 = sub nsw i32 %50, 1
  %52 = sext i32 %51 to i64
  %53 = getelementptr inbounds %struct.UnopEliminator, %struct.UnopEliminator* %47, i64 %52
  store %struct.UnopEliminator* %53, %struct.UnopEliminator** %11, align 8
  %54 = load %struct.Type*, %struct.Type** %7, align 8
  %55 = load %struct.UnopEliminator*, %struct.UnopEliminator** %11, align 8
  %56 = getelementptr inbounds %struct.UnopEliminator, %struct.UnopEliminator* %55, i32 0, i32 0
  store %struct.Type* %54, %struct.Type** %56, align 8
  %57 = load %struct.Type*, %struct.Type** %8, align 8
  %58 = load %struct.UnopEliminator*, %struct.UnopEliminator** %11, align 8
  %59 = getelementptr inbounds %struct.UnopEliminator, %struct.UnopEliminator* %58, i32 0, i32 1
  store %struct.Type* %57, %struct.Type** %59, align 8
  %60 = load void (%struct.Judgement*, %struct.Ast*)*, void (%struct.Judgement*, %struct.Ast*)** %9, align 8
  %61 = load %struct.UnopEliminator*, %struct.UnopEliminator** %11, align 8
  %62 = getelementptr inbounds %struct.UnopEliminator, %struct.UnopEliminator* %61, i32 0, i32 2
  store void (%struct.Judgement*, %struct.Ast*)* %60, void (%struct.Judgement*, %struct.Ast*)** %62, align 8
  br label %63

63:                                               ; preds = %27, %21
  %64 = load %struct.UnopEliminator*, %struct.UnopEliminator** %11, align 8
  store %struct.UnopEliminator* %64, %struct.UnopEliminator** %5, align 8
  br label %65

65:                                               ; preds = %63, %20
  %66 = load %struct.UnopEliminator*, %struct.UnopEliminator** %5, align 8
  ret %struct.UnopEliminator* %66
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local %struct.UnopEliminator* @FindUnopEliminator(%struct.UnopEliminatorList* %0, %struct.Type* %1) #0 {
  %3 = alloca %struct.UnopEliminator*, align 8
  %4 = alloca %struct.UnopEliminatorList*, align 8
  %5 = alloca %struct.Type*, align 8
  %6 = alloca %struct.UnopEliminator*, align 8
  %7 = alloca i32, align 4
  store %struct.UnopEliminatorList* %0, %struct.UnopEliminatorList** %4, align 8
  store %struct.Type* %1, %struct.Type** %5, align 8
  %8 = load %struct.UnopEliminatorList*, %struct.UnopEliminatorList** %4, align 8
  %9 = icmp ne %struct.UnopEliminatorList* %8, null
  br i1 %9, label %10, label %13

10:                                               ; preds = %2
  %11 = load %struct.Type*, %struct.Type** %5, align 8
  %12 = icmp ne %struct.Type* %11, null
  br i1 %12, label %14, label %13

13:                                               ; preds = %10, %2
  store %struct.UnopEliminator* null, %struct.UnopEliminator** %3, align 8
  br label %40

14:                                               ; preds = %10
  store %struct.UnopEliminator* null, %struct.UnopEliminator** %6, align 8
  store i32 0, i32* %7, align 4
  br label %15

15:                                               ; preds = %36, %14
  %16 = load i32, i32* %7, align 4
  %17 = load %struct.UnopEliminatorList*, %struct.UnopEliminatorList** %4, align 8
  %18 = getelementptr inbounds %struct.UnopEliminatorList, %struct.UnopEliminatorList* %17, i32 0, i32 1
  %19 = load i32, i32* %18, align 8
  %20 = icmp slt i32 %16, %19
  br i1 %20, label %21, label %39

21:                                               ; preds = %15
  %22 = load %struct.UnopEliminatorList*, %struct.UnopEliminatorList** %4, align 8
  %23 = getelementptr inbounds %struct.UnopEliminatorList, %struct.UnopEliminatorList* %22, i32 0, i32 0
  %24 = load %struct.UnopEliminator*, %struct.UnopEliminator** %23, align 8
  %25 = load i32, i32* %7, align 4
  %26 = sext i32 %25 to i64
  %27 = getelementptr inbounds %struct.UnopEliminator, %struct.UnopEliminator* %24, i64 %26
  store %struct.UnopEliminator* %27, %struct.UnopEliminator** %6, align 8
  %28 = load %struct.UnopEliminator*, %struct.UnopEliminator** %6, align 8
  %29 = getelementptr inbounds %struct.UnopEliminator, %struct.UnopEliminator* %28, i32 0, i32 0
  %30 = load %struct.Type*, %struct.Type** %29, align 8
  %31 = load %struct.Type*, %struct.Type** %5, align 8
  %32 = icmp eq %struct.Type* %30, %31
  br i1 %32, label %33, label %35

33:                                               ; preds = %21
  %34 = load %struct.UnopEliminator*, %struct.UnopEliminator** %6, align 8
  store %struct.UnopEliminator* %34, %struct.UnopEliminator** %3, align 8
  br label %40

35:                                               ; preds = %21
  br label %36

36:                                               ; preds = %35
  %37 = load i32, i32* %7, align 4
  %38 = add nsw i32 %37, 1
  store i32 %38, i32* %7, align 4
  br label %15, !llvm.loop !2

39:                                               ; preds = %15
  store %struct.UnopEliminator* null, %struct.UnopEliminator** %3, align 8
  br label %40

40:                                               ; preds = %39, %33, %13
  %41 = load %struct.UnopEliminator*, %struct.UnopEliminator** %3, align 8
  ret %struct.UnopEliminator* %41
}

; Function Attrs: nounwind
declare dso_local i8* @realloc(i8*, i64) #1

attributes #0 = { noinline nounwind optnone uwtable "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 12.0.0 (https://github.com/llvm/llvm-project/ b978a93635b584db380274d7c8963c73989944a1)"}
!2 = distinct !{!2, !3}
!3 = !{!"llvm.loop.mustprogress"}
