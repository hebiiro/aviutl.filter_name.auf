# 🎉AviUtl『フィルタ名調整』プラグイン

* [AviUtl](https://spring-fragrance.mints.ne.jp/aviutl/)のプラグインです。
* 設定ダイアログ内のアニメーション効果の表示名を調整します。

## ⬇️ダウンロード

* [最新バージョンはこちら](https://github.com/hebiiro/aviutl.filter_name.auf/releases/latest/)
* もしくは、[sigma-axis](https://github.com/sigma-axis)さんの[Reactive Dialog](https://github.com/sigma-axis/aviutl_reactive_dlg)にも同等の機能が存在するので、こちらを使用することもできます。

## 🚀導入方法

1. pluginsフォルダをaviutlフォルダにコピーします。

## 💡使い方

1. 設定ダイアログ内のアニメーション効果の表示名が自動的に調整されます。

## 🔧設定方法

* AviUtlを起動する前に`filter_name.ini`をテキストエディタで編集します。

### 📝設定項目

* `config`
	* `format` ✏️アニメーション効果の表示名の書式を指定します。
		* ✏️例えば`{}`と指定すると、`震える`と表示されます。
		* ✏️例えば`{} / アニメーション効果`と指定すると、`震える / アニメーション効果`と表示されます。
		* ✏️例えば`『{}』`と指定すると、`『震える』`と表示されます。
		* ✏️例えば`{:^60.40}`と指定すると、60文字の空白の中に中央寄せで`震える`と表示されます。ただし、文字数は最大で40文字に制限されます。(おそらく)
			* `^`の部分が`<`なら左寄せ、`>`なら右寄せになります。`{:<50}`のように`.`以降を省略することもできます。

## 🔖更新履歴

* r2 - 2024/07/10 🩹中間点が存在するアイテムだと正常に動作しない問題に対応
* r1 - 2024/06/30 🚀初版

## ⚗️動作確認

* [AviUtl 1.10 & 拡張編集 0.92](https://spring-fragrance.mints.ne.jp/aviutl/)

## 💳クレジット

* [aviutl_exedit_sdk](https://github.com/ePi5131/aviutl_exedit_sdk)
* [Microsoft Research Detours Package](https://github.com/microsoft/Detours)

## 🐣発案

* [兎](https://twitter.com/rabb_et) / [AviUtl知識共有(Discord)](https://t.co/PRrPkEHI3w)
	* https://discord.com/channels/1254055609578426499/1254056085619216436/1256590460617756683

## 🤝協力

* r2
	* [兎](https://twitter.com/rabb_et) / [AviUtl知識共有(Discord)](https://t.co/PRrPkEHI3w)
		* https://discord.com/channels/1254055609578426499/1254056085619216436/1259829474384416888

## 👽️作成者情報

* 作成者 - 蛇色 (へびいろ)
* GitHub - https://github.com/hebiiro
* Twitter - https://x.com/io_hebiiro

## 🚨免責事項

この作成物および同梱物を使用したことによって生じたすべての障害・損害・不具合等に関しては、私と私の関係者および私の所属するいかなる団体・組織とも、一切の責任を負いません。各自の責任においてご使用ください。
