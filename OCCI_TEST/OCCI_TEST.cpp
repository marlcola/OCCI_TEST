// OCCI_TEST.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string.h>
#include <oci.h>

int _tmain(int argc, _TCHAR* argv[])
{
	// OCI関数の戻り値
	sword status = OCI_SUCCESS;

	// 環境ハンドルの作成
	OCIEnv* envhp;
	status = OCIEnvCreate(
		&envhp,
		OCI_DEFAULT,
		0,
		0,
		0,
		0,
		0,
		0
	);
	if (status != OCI_SUCCESS)
	{
		printf("OCIEnvCreate - error : %d.\n", status);
		return -1;
	}

	//エラーハンドルの作成
	OCIError* errhp;
	status = OCIHandleAlloc(
		envhp, 
		(dvoid **)&errhp, 
		OCI_HTYPE_ERROR, 
		0, 
		0
	);
	if (status != OCI_SUCCESS)
	{
		printf("OCIHandleAlloc - error : %d.\n", status);
		return -1;
	}

	// 接続記述子
	char *dblink = "mori";
	
	// サーバハンドルの作成
	OCIServer *srvhp;
	status = OCIHandleAlloc(envhp, (dvoid **)&srvhp, OCI_HTYPE_SERVER, 0, 0);
	if (status != OCI_SUCCESS)
	{
		printf("OCIHandleAlloc - error : %d.\n", status);
		return -1;
	}
	status = OCIServerAttach(
		srvhp,
		errhp,
		(OraText*)dblink,
		strlen(dblink),
		OCI_DEFAULT
	);
	if (status != OCI_SUCCESS)
	{
		printf("OCIHandleAlloc - error : %d.\n", status);
		return -1;
	}

	// ログイン情報
	char* username = "mori";
	char* password = "Cola3101";
	char* dbname = "mori";

	// サーバコンテキストハンドルの作成
	OCISvcCtx* svchp;
	status = OCILogon(
		envhp, 
		errhp, 
		&svchp, 
		(OraText*)username, 
		strlen(username),
		(OraText*)password, 
		strlen(password), 
		(OraText*)dbname, 
		//NULL,
		strlen(dbname)
		//0
	);
	if (status != OCI_SUCCESS)
	{
		printf("OCILogon - error : %d.\n", status);
		return -1;
	}

	// SQL
	char* stmt = "select num, name from table01";

	// 文ハンドルの作成
	OCIStmt* stmtp;
	status = OCIHandleAlloc(envhp, (dvoid **)&stmtp, OCI_HTYPE_STMT, 0, 0);
	if (status != OCI_SUCCESS)
	{
		printf("OCIHandleAlloc - error : %d.\n", status);
		goto finallyFree;
	}
	status = OCIStmtPrepare(stmtp, errhp, (OraText*)stmt, strlen(stmt), OCI_NTV_SYNTAX, OCI_DEFAULT);

	if (status != OCI_SUCCESS)
	{
		printf("OCIStmtPrepare - error : %d.\n", status);
		goto finallyFree;
	}

	// 検索結果の入れ物
	char message1[64] = { 0 };
	char message2[64] = { 0 };

	// 定義ハンドルの作成
	OCIDefine* dfnp, *dfnp2;
	status = OCIDefineByPos(
		stmtp, 
		&dfnp, 
		errhp, 
		1, 
		message1, 
		sizeof(message1), 
		SQLT_STR, 
		0, 
		0, 
		0, 
		OCI_DEFAULT
	);
	status = OCIDefineByPos(
		stmtp, 
		&dfnp2, 
		errhp, 
		2, 
		message2, 
		sizeof(message2), 
		SQLT_STR, 
		0, 
		0, 
		0, 
		OCI_DEFAULT
	);
	if (status != OCI_SUCCESS)
	{
		printf("OCIDefineByPos - error : %d.\n", status);
		goto finallyFree;
	}

	
	// SQL文の実行
	status = OCIStmtExecute(svchp, stmtp, errhp, 1, 0, NULL, NULL, OCI_DEFAULT);
	if (status != OCI_SUCCESS)
	{
		printf("OCIStmtExecute - error : %d.\n", status);
		goto finallyFree;
	}

	// 検索結果出力
	while (status == OCI_SUCCESS)
	{
		printf("%s\n", message1);
		printf("%s\n", message2);
		// 結果セットから行をフェッチする
		status = OCIStmtFetch2(stmtp, errhp, 1, OCI_FETCH_NEXT, 0, OCI_DEFAULT);
	}

// 各種ハンドルの開放
finallyFree:
	// ハンドル解放
	OCIHandleFree(stmtp, OCI_HTYPE_STMT);
	// OCILogonを使用して作成したログイン・セッションを解放する
	OCILogoff(svchp, errhp);
	// ハンドル解放
	OCIHandleFree(errhp, OCI_HTYPE_ERROR);

    return 0;
}

